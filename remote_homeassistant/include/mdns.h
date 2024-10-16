#ifndef mdns_client_h
#define mdns_client_h

#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoMDNS.h>

/**
 * @brief Set the up mdns object
 *
 * @param mdns
 * @param callback
 */
void setup_mdns(MDNS &mdns, void (*callback)(const char *, IPAddress));

/**
 * @brief Run recursively until hostname is found
 *
 * @param mdns
 * @param hostname
 */
void loop_mdns(MDNS &mdns, const char *hostname);

#endif
