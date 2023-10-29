#ifndef defines_h
#define defines_h

#include "secrets.h"
#include "remote_rf.h"

#if !( defined(ARDUINO_RASPBERRY_PI_PICO_W) )
  #error For RASPBERRY_PI_PICO_W only
#endif

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

// Define constants for modulation types
#define MODULATION_2_FSK 0
#define MODULATION_GFSK 1
#define MODULATION_ASK_OOK 2
#define MODULATION_4_FSK 3
#define MODULATION_MSK 4

// Define frequency bands
#define F868 868.23
#define F433 433.92

typedef struct {
    const char* id;
    float frequency;
    byte modulation;
    const char* code;
    int replays;
    int symbolDuration_usec;
} Remote_t;


// secrets.h:
// #define WIFI_SSID         "name"    // your network SSID (name)
// #define WIFI_PASSWORD     "passwd"    // your network password (use for WPA, or use as key for WEP), length must be 8+

// #define MQTT_USER         "user";
// #define MQTT_CREDENTIALS  "credentials";


#endif    //defines_h
