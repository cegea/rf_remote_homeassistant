#ifndef mdns_client_h
#define mdns_client_h

#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoMDNS.h>

void setup_mdns(MDNS& mdns, void (*callback)(const char*, IPAddress)) ;
void loop_mdns(MDNS& mdns, const char* hostname);

#endif 
