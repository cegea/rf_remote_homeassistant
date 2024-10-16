#include <WebServer.h>
#include <WiFi.h>
#include "RP2040.h"
#include "provisioning.h"
#include <EEPROM.h>
#include "html_content.h"

wifi_settings __user_wifi = {};
mqtt_settings __user_mqtt = {};

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

// Global
WebServer server(80);

// Functions
void __print_credentials()
{
#ifdef DEBUG_PROVISIONING
    DEBUG_APPLICATION_PORT.println("\nWiFi Settings:");
    DEBUG_APPLICATION_PORT.print("SSID: ");
    DEBUG_APPLICATION_PORT.println(__user_wifi.ssid);
    DEBUG_APPLICATION_PORT.print("MQTT Host: ");
    DEBUG_APPLICATION_PORT.println(__user_mqtt.host);
    DEBUG_APPLICATION_PORT.print("MQTT Port: ");
    DEBUG_APPLICATION_PORT.println(__user_mqtt.port);
    DEBUG_APPLICATION_PORT.print("MQTT User: ");
    DEBUG_APPLICATION_PORT.println(__user_mqtt.user);
#endif
}

void __handlePortal()
{
    char port[10];
    if (server.method() == HTTP_POST)
    {

        strncpy(__user_wifi.ssid, server.arg("ssid").c_str(), sizeof(__user_wifi.ssid));
        __user_wifi.ssid[server.arg("ssid").length()] = '\0';

        strncpy(__user_wifi.password, server.arg("password").c_str(), sizeof(__user_wifi.password));
        __user_wifi.password[server.arg("password").length()] = '\0';

        strncpy(__user_mqtt.host, server.arg("mqtt_host").c_str(), sizeof(__user_mqtt.host));
        __user_mqtt.host[server.arg("mqtt_host").length()] = '\0';

        strncpy(port, server.arg("mqtt_port").c_str(), sizeof(port));
        __user_mqtt.port = atoi(port);

        strncpy(__user_mqtt.user, server.arg("mqtt_user").c_str(), sizeof(__user_mqtt.user));
        __user_mqtt.user[server.arg("mqtt_user").length()] = '\0';

        strncpy(__user_mqtt.passwd, server.arg("mqtt_pswd").c_str(), sizeof(__user_mqtt.passwd));
        __user_mqtt.passwd[server.arg("mqtt_pswd").length()] = '\0';

        __print_credentials();

        EEPROM.put(__WIFI_SETTINGS_ADDR, __user_wifi);
        EEPROM.put(__MQTT_SETTINGS_ADDR, __user_mqtt);
        EEPROM.commit();

        server.send(200, "text/html", HTML_SUCCESS_PAGE);
    }
    else
    {
        server.send(200, "text/html", HTML_PAGE);
    }
}

void __wifi_ap()
{

#ifdef DEBUG_PROVISIONING
    DEBUG_APPLICATION_PORT.println("\nTry to connect to WIFI");
#endif
    __print_credentials();

    // WiFi.mode(WIFI_STA);
    WiFi.begin(__user_wifi.ssid, __user_wifi.password);
}

void provisioning_delete_credentials()
{
    memset(__user_wifi.ssid, 0, sizeof(__user_wifi.ssid));
    memset(__user_wifi.password, 0, sizeof(__user_wifi.password));
    memset(__user_mqtt.host, 0, sizeof(__user_mqtt.host));
    __user_mqtt.port = 0;
    memset(__user_mqtt.user, 0, sizeof(__user_mqtt.user));
    memset(__user_mqtt.passwd, 0, sizeof(__user_mqtt.passwd));

    EEPROM.put(__WIFI_SETTINGS_ADDR, __user_wifi);
    EEPROM.put(__MQTT_SETTINGS_ADDR, __user_mqtt);
    EEPROM.commit();
}

void command_server_provisioning()
{
    if (DEBUG_APPLICATION_PORT.available() > 0)
    {
        String command = DEBUG_APPLICATION_PORT.readStringUntil('\n'); // Read until a newline is encountered

        // Remove any leading or trailing whitespace
        command.trim();

        if (command.equals("--delete_wifi_credentials"))
        {
            provisioning_delete_credentials();
            DEBUG_APPLICATION_PORT.println("Credentials erased.");
        }
        else
        {
            DEBUG_APPLICATION_PORT.println("Command not recognized.");
        }
    }
}

mqtt_settings read_EEPROM_mqtt_credentials()
{
    EEPROM.get(__MQTT_SETTINGS_ADDR, __user_mqtt);
    return __user_mqtt;
}

wifi_settings read_EEPROM_wifi_credentials()
{
    EEPROM.get(__WIFI_SETTINGS_ADDR, __user_wifi);
    return __user_wifi;
}

void provisioning_setup()
{

#ifdef DEBUG_PROVISIONING
    DEBUG_APPLICATION_PORT.print("\nStart credentials provisioning");
#endif
    read_EEPROM_wifi_credentials();
    read_EEPROM_mqtt_credentials();

    __wifi_ap();

    byte tries = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        if (tries++ > 3)
        {
            WiFi.mode(WIFI_AP);
            WiFi.softAP("Remote Provisioning", "provisioning");
#ifdef DEBUG_PROVISIONING
            DEBUG_APPLICATION_PORT.println("\nWiFi AP(Remote Provisioning, provisioning)");
#endif
            break;
        }
    }
    server.on("/", __handlePortal);
    server.begin();
}

void provisioning_loop()
{
    if (WiFi.getMode() >= WIFI_AP)
    {
        command_server_provisioning();
        server.handleClient();
    }
    else if (WiFi.getMode() == WIFI_STA && WiFi.status() == WL_CONNECTED)
    {
        server.close();
    }
}
