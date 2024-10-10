#ifndef provisioning_h
#define provisioning_h

#include "main.h"

// Type def
struct wifi_settings {
  char ssid[30];
  char password[30];
} ;

struct mqtt_settings {
  char host[30];
  uint16_t port;
  char user[30];
  char passwd[30];
} ;

// Defines
#define __WIFI_SETTINGS_SIZE sizeof(struct wifi_settings)
#define __MQTT_SETTINGS_SIZE sizeof(struct mqtt_settings)
#define __PROVISIONING_EEPROM_SIZE PROVISIONING_SIZE
#define __WIFI_SETTINGS_ADDR PROVISIONING_ADDR
#define __MQTT_SETTINGS_ADDR PROVISIONING_ADDR + __WIFI_SETTINGS_SIZE

static_assert(__WIFI_SETTINGS_SIZE + __MQTT_SETTINGS_SIZE <= __PROVISIONING_EEPROM_SIZE, "Max size for provisioning reached");

void provisioning_setup(void);
void provisioning_loop(void);
void clean_wifi_credentials(void);

/**
 * @brief Read from EEPROM the MQTT data
 * 
 * @return mqtt_settings 
 */
mqtt_settings read_EEPROM_mqtt_credentials(void);

/**
 * @brief Read from EEPROM the MQTT data
 * 
 * @return wifi_settings 
 */
wifi_settings read_EEPROM_wifi_credentials(void);

#endif