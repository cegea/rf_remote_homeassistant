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
#ifdef ARDUINO_RASPBERRY_PI_PICO_W
volatile Remote_t remoteControl PSRAM;
#elif defined(ARDUINO_LOLIN32_LITE)
volatile Remote_t remoteControl;
#endif

/**
 * @brief Initialize Serial port
 * 
 */
void __init_serial(void);

void __init_serial(void){
#ifdef ARDUINO_RASPBERRY_PI_PICO_W
	DEBUG_APPLICATION_PORT.setRX(1);
	DEBUG_APPLICATION_PORT.setTX(0);
	DEBUG_APPLICATION_PORT.begin(9600);
#elif defined(ARDUINO_LOLIN32_LITE)
	DEBUG_APPLICATION_PORT.begin(9600);
#endif
}

#ifndef ARDUINO_LOLIN32_LITE
void setup1()
{
	__init_serial();
	DEBUG_APPLICATION_PORT.println("\n[C1]Starting REMOTE RF");

	remote.cc1101initialize();
}

void loop1()
{
	remote.processIncomingCommands();
}
#endif

void setup()
{
	__init_serial();
	EEPROM.begin(EEPROM_SIZE);

#ifdef ARDUINO_LOLIN32_LITE
	DEBUG_APPLICATION_PORT.println("\n[C1]Starting REMOTE RF");
	remote.cc1101initialize();
#endif

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
#ifdef ARDUINO_LOLIN32_LITE
	remote.processIncomingCommands();
#endif
}
