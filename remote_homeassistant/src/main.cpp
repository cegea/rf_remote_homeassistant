#include <Arduino.h>
#include "main.h"
#include "mqtt_client.h"
#include "remote_rf.h"
#include "provisioning.h"
#include <WiFi.h>
#include "mdns.h"
#include <EEPROM.h>
#include "scheduler.h"

// Declare remoteRF

RemoteRF remote(SCK, MISO, MOSI, SS, GDO0, GDO2);
volatile Remote_t remoteControl PSRAM;

void setup1()
{
	DEBUG_APPLICATION_PORT.setRX(1);
	DEBUG_APPLICATION_PORT.setTX(0);
	DEBUG_APPLICATION_PORT.begin(9600);

	DEBUG_APPLICATION_PORT.println("\n[C1]Starting REMOTE RF");

	remote.cc1101initialize();
}

void loop1()
{
	remote.processIncomingCommands();
}

void setup()
{
	DEBUG_APPLICATION_PORT.setRX(1);
	DEBUG_APPLICATION_PORT.setTX(0);
	DEBUG_APPLICATION_PORT.begin(9600);

	EEPROM.begin(EEPROM_SIZE);

	DEBUG_APPLICATION_PORT.println("\n[C0]Starting REMOTE RF");

	provisioning_setup();

	if (WiFi.status() != WL_CONNECTED)
	{
		// Follow provisioning and restart
		// TODO: Avoid manual restarting if possible. NVIC_SystemReset();
	}
	else if (WiFi.status() == WL_CONNECTED)
	{
		setup_mqtt();
		setup_scheduler();
	}
}

void loop()
{
	// DEBUG_APPLICATION_PORT.println(WiFi.status());
	if (WiFi.getMode() >= WIFI_AP)
	{
		provisioning_loop();
	}
	else if (WiFi.getMode() == WIFI_STA && WiFi.status() == WL_CONNECTED)
	{
		loop_mqtt();
	}
}
