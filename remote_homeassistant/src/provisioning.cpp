#include <WebServer.h>
#include <WiFi.h>
#include "RP2040.h"
#include "provisioning.h"
#include <EEPROM.h>
#include "html_content.h"

wifi_settings __user_wifi = {};
mqtt_settings __user_mqtt = {};

void __copy_string(char *destination, size_t destinationSize, const String &source)
{
	const size_t copyLength = min(destinationSize - 1, source.length());
	memcpy(destination, source.c_str(), copyLength);
	destination[copyLength] = '\0';
}

String __normalize_mqtt_host(String mqttHost, uint16_t *mqttPort)
{
	mqttHost.trim();

	const int schemeSeparator = mqttHost.indexOf("://");
	if (schemeSeparator >= 0)
	{
		mqttHost.remove(0, schemeSeparator + 3);
	}

	const int credentialsSeparator = mqttHost.lastIndexOf('@');
	if (credentialsSeparator >= 0)
	{
		mqttHost.remove(0, credentialsSeparator + 1);
	}

	const int pathSeparator = mqttHost.indexOf('/');
	if (pathSeparator >= 0)
	{
		mqttHost.remove(pathSeparator);
	}

	const int querySeparator = mqttHost.indexOf('?');
	if (querySeparator >= 0)
	{
		mqttHost.remove(querySeparator);
	}

	const int firstColon = mqttHost.indexOf(':');
	const int lastColon = mqttHost.lastIndexOf(':');
	if (firstColon > 0 && firstColon == lastColon)
	{
		const String portText = mqttHost.substring(lastColon + 1);
		bool isNumericPort = portText.length() > 0;

		for (size_t index = 0; index < portText.length(); index++)
		{
			if (!isDigit(portText[index]))
			{
				isNumericPort = false;
				break;
			}
		}

		if (isNumericPort)
		{
			if (*mqttPort == 0)
			{
				*mqttPort = static_cast<uint16_t>(atoi(portText.c_str()));
			}
			mqttHost.remove(lastColon);
		}
	}

	return mqttHost;
}

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
 * @brief Compare wifi setting and return 0 if equal
 * 
 * @param wifi1 
 * @param wifi2 
 * @return uint8_t 
 */
uint8_t __cmp_wifi_settings(wifi_settings wifi1, wifi_settings wifi2);

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
	if (server.method() == HTTP_POST)
	{
		String mqttHost = server.arg("mqtt_host");
		String mqttPort = server.arg("mqtt_port");
		uint16_t parsedMqttPort = 0;

		mqttPort.trim();
		if (mqttPort.length() > 0)
		{
			parsedMqttPort = static_cast<uint16_t>(atoi(mqttPort.c_str()));
		}

		mqttHost = __normalize_mqtt_host(mqttHost, &parsedMqttPort);

		__copy_string(__user_wifi.ssid, sizeof(__user_wifi.ssid), server.arg("ssid"));

		__copy_string(__user_wifi.password, sizeof(__user_wifi.password), server.arg("password"));

		__copy_string(__user_mqtt.host, sizeof(__user_mqtt.host), mqttHost);

		__user_mqtt.port = parsedMqttPort;

		__copy_string(__user_mqtt.user, sizeof(__user_mqtt.user), server.arg("mqtt_user"));

		__copy_string(__user_mqtt.passwd, sizeof(__user_mqtt.passwd), server.arg("mqtt_pswd"));

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

uint8_t __cmp_wifi_settings(wifi_settings wifi1, wifi_settings wifi2){
	for (size_t i = 0; i < SSID_LEN; i++)
	{	
		if (wifi1.ssid[i] != wifi2.ssid[i])
		{
			return 1;
		}
		
	}
	return 0;
	
}

bool check_provisioning_done(){
	wifi_settings empty_wifi = {};

	memset(empty_wifi.ssid, 0xFF, sizeof(empty_wifi.ssid));
	memset(empty_wifi.password, 0xFF, sizeof(empty_wifi.password));

	if (__cmp_wifi_settings(__user_wifi, empty_wifi) == 0)
	{
		return false;
	}
	else{
		return true;
	}
}

void provisioning_setup()
{

#ifdef DEBUG_PROVISIONING
	DEBUG_APPLICATION_PORT.print("\nStart credentials provisioning");
#endif
	read_EEPROM_wifi_credentials();
	read_EEPROM_mqtt_credentials();

	if (check_provisioning_done())
	{
		__wifi_ap();
	}
	

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
