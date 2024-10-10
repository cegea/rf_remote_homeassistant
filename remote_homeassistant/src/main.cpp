#include <Arduino.h>
#include "main.h"
#include "mqtt_client.h"
#include "remote_rf.h"
#include "provisioning.h"
#include <WiFi.h>
#include "mdns.h"
#include <EEPROM.h>


//Declare remoteRF

RemoteRF remote(SCK, MISO, MOSI, SS, GDO0, GDO2);

// Declare remotes:
Remote_t remoteControlArray[] = {
    {"CHISME_ON", F433, MODULATION_ASK_OOK, CODE_CHISME_ON, 4, 300},
    {"CHISME_OFF", F433, MODULATION_ASK_OOK, CODE_CHISME_OFF, 4, 300},
    {"INT_DOOR", F868, MODULATION_ASK_OOK, CODE_INTERIOR_DOOR, 8, 215},
    {"EXT_DOOR", F868, MODULATION_ASK_OOK, CODE_EXTERIOR_DOOR, 8, 215}
};

// Get the size of the remoteControlArray
size_t remoteControlArraySize = sizeof(remoteControlArray) / sizeof(remoteControlArray[0]);

// void setup1() {
//   remote.cc1101initialize();
// }

// void loop1() {
//   remote.processIncomingCommands(remoteControlArray,remoteControlArraySize);
// }

void setup()
{
	Serial1.setRX(1);
    Serial1.setTX(0);
    Serial1.begin(9600);

    EEPROM.begin(EEPROM_SIZE); 

    Serial1.println("\nStarting REMOTE RF");

	provisioning_setup();

	if (WiFi.status() != WL_CONNECTED)
	{
		// Follow provisioning and restart
		// TODO: Avoid manual restarting if possible. NVIC_SystemReset();
	}
	else if (WiFi.status() == WL_CONNECTED)
	{
		setup_mqtt();
	}
}

void loop()
{
	// Serial1.println(WiFi.status());
	if (WiFi.getMode() >= WIFI_AP)
	{
		provisioning_loop();
	}
	else if (WiFi.getMode() == WIFI_STA && WiFi.status() == WL_CONNECTED)
	{
		loop_mqtt();
	}
  	// Serial1.print("\nLooping Main");
  	// Serial1.print("\nWiFi.getMode()");
  	// Serial1.print(WiFi.getMode());
  	// Serial1.print("\nWiFi.status()");
  	// Serial1.print(WiFi.status());

}
