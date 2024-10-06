#ifndef mdns_client_h
#define mdns_client_h

#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoMDNS.h>

// void resolver_callback(const char* name, const IPAddress& ip);
void setup_mdns(MDNS& mdns, void (*callback)(const char*, IPAddress)) ;
void loop_mdns(MDNS& mdns, const char* hostname);

#endif 
