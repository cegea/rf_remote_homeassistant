#ifndef defines_h
#define defines_h

#include "secrets.h"

#if !( defined(ARDUINO_RASPBERRY_PI_PICO_W) )
  #error For RASPBERRY_PI_PICO_W only
#endif

// Debug Level from 0 to 4
#define _ASYNCTCP_RP2040W_LOGLEVEL_         1
#define _ASYNC_MQTT_LOGLEVEL_               1

#if (_ASYNC_MQTT_LOGLEVEL_ > 3)
  #warning Using RASPBERRY_PI_PICO_W with CYC43439 WiFi
#endif

// secrets.h:
// #define WIFI_SSID         "name"    // your network SSID (name)
// #define WIFI_PASSWORD     "passwd"    // your network password (use for WPA, or use as key for WEP), length must be 8+

// #define MQTT_USER         "user";
// #define MQTT_CREDENTIALS  "credentials";


#endif    //defines_h
