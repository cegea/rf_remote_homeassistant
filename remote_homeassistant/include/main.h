#ifndef main_h
#define main_h

#include "secrets.h"
#include "remote_rf.h"

// #if !( defined(ARDUINO_RASPBERRY_PI_PICO_W) )
//   #error For RASPBERRY_PI_PICO_W only
// #endif

// Debug Level from 0 to 4
#define _ASYNCTCP_RP2040W_LOGLEVEL_         1
#define _ASYNC_MQTT_LOGLEVEL_               1

#if (_ASYNC_MQTT_LOGLEVEL_ > 3)
  #warning Using RASPBERRY_PI_PICO_W with CYC43439 WiFi
#endif

// RP2040 pins SPI0
#define SCK 2
#define MISO 4
#define MOSI 3
#define SS 5
#define GDO0 7
#define GDO2 6

// EEPROM MAP (In RP204 is 4k of flash)
#define PROVISIONING_ADDR 0
#define PROVISIONING_SIZE 256
#define MQTT_ADDR PROVISIONING_SIZE
#define MQTT_SIZE 128
#define RADIO_ADDR MQTT_ADDR + MQTT_SIZE
#define RADIO_SIZE 1024
#define EEPROM_SIZE PROVISIONING_SIZE + MQTT_SIZE + RADIO_SIZE

static_assert(PROVISIONING_ADDR == 0, "Do not change the map randomly");
static_assert(MQTT_ADDR == PROVISIONING_ADDR + PROVISIONING_SIZE, "MQTT address not properly calculated");
static_assert(RADIO_ADDR == PROVISIONING_ADDR + PROVISIONING_SIZE + MQTT_SIZE, "Radio address not properly calculated");



#endif    //main_h
