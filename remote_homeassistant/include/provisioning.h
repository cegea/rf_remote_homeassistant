#ifndef provisioning_h
#define provisioning_h

#include "main.h"

#define SSID_LEN 30
#define PWD_LEN 30

// Type def
struct wifi_settings
{
  char ssid[SSID_LEN];
  char password[PWD_LEN];
};

struct mqtt_settings
{
  char host[30];
  uint16_t port;
  char user[30];
  char passwd[30];
};

// Defines
#define __WIFI_SETTINGS_SIZE sizeof(struct wifi_settings)
#define __MQTT_SETTINGS_SIZE sizeof(struct mqtt_settings)
#define __PROVISIONING_EEPROM_SIZE PROVISIONING_SIZE
#define __WIFI_SETTINGS_ADDR PROVISIONING_ADDR
#define __MQTT_SETTINGS_ADDR PROVISIONING_ADDR + __WIFI_SETTINGS_SIZE

static_assert(__WIFI_SETTINGS_SIZE + __MQTT_SETTINGS_SIZE <= __PROVISIONING_EEPROM_SIZE, "Max size for provisioning reached");

/**
 * @brief Setup function for provisioning
 *
 */
void provisioning_setup(void);

/**
 * @brief Main loop of provisioning application
 *
 */
void provisioning_loop(void);

/**
 * @brief Delete WiFi and MQTT credentials
 *
 */
void provisioning_delete_credentials(void);

/**
 * @brief Command server over UART
 *
 */
void command_server_provisioning(void);

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

/**
 * @brief Read EEPROM and compare with empty EEPROM values
 * 
 * @return true When there are credentials
 * @return false When there are no credentials
 */
bool check_provisioning_done(void);

#endif