#include <WebServer.h>
#include <WiFi.h>
#include "RP2040.h"
#include "provisioning.h"
#include <EEPROM.h>
#include "html_content.h"

// Type def
struct wifi_settings {
  char ssid[30];
  char password[30];
} user_wifi = {};

struct mqtt_settings {
  char host[30];
  uint16_t port;
  char user[30];
  char passwd[30];
} user_mqtt = {};

// Private functions
void __handlePortal(void);
void __wifi_ap(void);
void __print_credentials(void);
void __check_for_serial_commands(void);

// Global
WebServer server(80);

// Functions
void __print_credentials(){
    Serial1.println("\nWiFi Settings:");
    Serial1.print("SSID: ");
    Serial1.println(user_wifi.ssid);
    Serial1.print("MQTT Host: ");
    Serial1.println(user_mqtt.host);
    Serial1.print("MQTT Port: ");
    Serial1.println(user_mqtt.port);
    Serial1.print("MQTT User: ");
    Serial1.println(user_mqtt.user);
}

void __handlePortal(){
    char port[10];
    if (server.method() == HTTP_POST) {
    
        strncpy(user_wifi.ssid,     server.arg("ssid").c_str(),     sizeof(user_wifi.ssid) );
        user_wifi.ssid[server.arg("ssid").length()] = '\0';

        strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password) );
        user_wifi.password[server.arg("password").length()] = '\0';

        strncpy(user_mqtt.host, server.arg("mqtt_host").c_str(), sizeof(user_mqtt.host) );
        user_mqtt.host[server.arg("mqtt_host").length()] = '\0';

        strncpy(port, server.arg("mqtt_port").c_str(), sizeof(port) );
        user_mqtt.port = atoi(port);

        strncpy(user_mqtt.user, server.arg("mqtt_user").c_str(), sizeof(user_mqtt.user) );
        user_mqtt.user[server.arg("mqtt_user").length()] = '\0';
        
        strncpy(user_mqtt.passwd, server.arg("mqtt_pswd").c_str(), sizeof(user_mqtt.passwd) );
        user_mqtt.passwd[server.arg("mqtt_pswd").length()] = '\0';

        __print_credentials();

        EEPROM.put(0, user_wifi);
        EEPROM.put( sizeof(struct wifi_settings), user_mqtt );
        EEPROM.commit();

        Serial1.println("\nKeys updated");

        server.send(200,   "text/html",  HTML_SUCCESS_PAGE);
    } 
    else {
        server.send(200,   "text/html", HTML_PAGE);
    }
}

void __wifi_ap(){

    Serial1.println("\nTry to connect to WIFI");
    __print_credentials();

    // WiFi.mode(WIFI_STA);
    WiFi.begin(user_wifi.ssid, user_wifi.password);
}

void __clean_wifi_credentials(){
    memset(user_wifi.ssid, 0, sizeof(user_wifi.ssid));
    memset(user_wifi.password, 0, sizeof(user_wifi.password));
    memset(user_mqtt.host, 0, sizeof(user_mqtt.host));
    user_mqtt.port = 0;
    memset(user_mqtt.user, 0, sizeof(user_mqtt.user));
    memset(user_mqtt.passwd, 0, sizeof(user_mqtt.passwd));

    EEPROM.put(0, user_wifi);
    EEPROM.put( sizeof(struct wifi_settings), user_mqtt );
    EEPROM.commit();
}

void __check_for_serial_commands() {
    if (Serial1.available() > 0) {
        String command = Serial1.readStringUntil('\n');  // Read until a newline is encountered
        
        // Remove any leading or trailing whitespace
        command.trim();

        if (command.equals("--delete_wifi_credentials")) {
            __clean_wifi_credentials();
            Serial1.println("Credentials erased.");
        } else {
            Serial1.println("Command not recognized.");
        }
    }
}

void provisioning_setup(){

    Serial1.setRX(1);
    Serial1.setTX(0);
    Serial1.begin(9600);

    Serial1.print("\nStarting REMOTE RF");

    EEPROM.begin(sizeof(struct wifi_settings) + sizeof(struct mqtt_settings) );
    EEPROM.get( 0, user_wifi );
    EEPROM.get( sizeof(struct wifi_settings), user_mqtt );

    __wifi_ap();

    byte tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        if (tries++ > 3) {
            WiFi.mode(WIFI_AP);
            WiFi.softAP("Remote Provisioning", "provisioning");
            Serial1.println("\nWiFi AP(Remote Provisioning, provisioning)");
            break;
        }
    }
  server.on("/",  __handlePortal);
  server.begin();
}

void provisioning_loop(){
    // Serial1.println(WiFi.status());
    if(WiFi.getMode() >= WIFI_AP)
    {
        __check_for_serial_commands();
        server.handleClient();
    }
    else if (WiFi.getMode() == WIFI_STA && WiFi.status() == WL_CONNECTED)
    {
        server.close();
        // Serial1.end(); // Close debug resource for others to use
    }
    

}
