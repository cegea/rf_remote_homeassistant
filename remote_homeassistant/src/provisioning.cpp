#include <WebServer.h>
#include <WiFi.h>
#include "RP2040.h"
#include "provisioning.h"
#include <EEPROM.h>
#include "html_content.h"

wifi_settings user_wifi = {};
mqtt_settings user_mqtt = {};

// Private functions

/**
 * @brief Serve HTTP requests from clients
 * 
 */
void __handlePortal(void);

/**
 * @brief Try to connect to WiFi with the stored provided.
 * 
 */
void __wifi_ap(void);

/**
 * @brief Output stored credentials over UART
 * 
 */
void __print_credentials(void);

/**
 * @brief Command server over UART
 * 
 */
void command_server_provisioning(void);

// Global
WebServer server(80);

// Functions
void __print_credentials(){
#ifdef DEBUG_PROVISIONING
    DEBUG_RP2040_PORT.println("\nWiFi Settings:");
    DEBUG_RP2040_PORT.print("SSID: ");
    DEBUG_RP2040_PORT.println(user_wifi.ssid);
    DEBUG_RP2040_PORT.print("MQTT Host: ");
    DEBUG_RP2040_PORT.println(user_mqtt.host);
    DEBUG_RP2040_PORT.print("MQTT Port: ");
    DEBUG_RP2040_PORT.println(user_mqtt.port);
    DEBUG_RP2040_PORT.print("MQTT User: ");
    DEBUG_RP2040_PORT.println(user_mqtt.user);
#endif
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

        EEPROM.put(__WIFI_SETTINGS_ADDR, user_wifi);
        EEPROM.put(__MQTT_SETTINGS_ADDR, user_mqtt );
        EEPROM.commit();

        server.send(200,   "text/html",  HTML_SUCCESS_PAGE);
    } 
    else {
        server.send(200,   "text/html", HTML_PAGE);
    }
}

void __wifi_ap(){

#ifdef DEBUG_PROVISIONING
    DEBUG_RP2040_PORT.println("\nTry to connect to WIFI");
#endif
    __print_credentials();

    // WiFi.mode(WIFI_STA);
    WiFi.begin(user_wifi.ssid, user_wifi.password);
}

void clean_wifi_credentials(){
    memset(user_wifi.ssid, 0, sizeof(user_wifi.ssid));
    memset(user_wifi.password, 0, sizeof(user_wifi.password));
    memset(user_mqtt.host, 0, sizeof(user_mqtt.host));
    user_mqtt.port = 0;
    memset(user_mqtt.user, 0, sizeof(user_mqtt.user));
    memset(user_mqtt.passwd, 0, sizeof(user_mqtt.passwd));

    EEPROM.put(__WIFI_SETTINGS_ADDR, user_wifi);
    EEPROM.put(__MQTT_SETTINGS_ADDR, user_mqtt );
    EEPROM.commit();
}

void command_server_provisioning() {
    if (DEBUG_RP2040_PORT.available() > 0) {
        String command = DEBUG_RP2040_PORT.readStringUntil('\n');  // Read until a newline is encountered
        
        // Remove any leading or trailing whitespace
        command.trim();

        if (command.equals("--delete_wifi_credentials")) {
            clean_wifi_credentials();
            DEBUG_RP2040_PORT.println("Credentials erased.");
        } else {
            DEBUG_RP2040_PORT.println("Command not recognized.");
        }
    }
}

mqtt_settings read_EEPROM_mqtt_credentials(){
    EEPROM.get(__MQTT_SETTINGS_ADDR, user_mqtt );
    return user_mqtt;
}

wifi_settings read_EEPROM_wifi_credentials(){
    EEPROM.get(__WIFI_SETTINGS_ADDR, user_wifi );
    return user_wifi;
}

void provisioning_setup(){

#ifdef DEBUG_PROVISIONING
    DEBUG_RP2040_PORT.print("\nStart credentials provisioning");
#endif
    read_EEPROM_wifi_credentials();
    read_EEPROM_mqtt_credentials();

    __wifi_ap();

    byte tries = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        if (tries++ > 3) {
            WiFi.mode(WIFI_AP);
            WiFi.softAP("Remote Provisioning", "provisioning");
#ifdef DEBUG_PROVISIONING
            DEBUG_RP2040_PORT.println("\nWiFi AP(Remote Provisioning, provisioning)");
#endif
            break;
        }
    }
  server.on("/",  __handlePortal);
  server.begin();
}

void provisioning_loop(){
    if(WiFi.getMode() >= WIFI_AP)
    {
        command_server_provisioning();
        server.handleClient();
    }
    else if (WiFi.getMode() == WIFI_STA && WiFi.status() == WL_CONNECTED)
    {
        server.close();
    }
    

}
