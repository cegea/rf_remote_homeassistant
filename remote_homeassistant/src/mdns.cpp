#include "mdns.h"

#define TIMEOUT 5000

// Define the callback function signature (based on what ArduinoMDNS expects)
// void resolver_callback(const char* name, const IPAddress& ip) {
//   // Handle the name resolution result here
//   Serial.print("Resolved: ");
//   Serial.print(name);
//   Serial.print(" to IP: ");
//   Serial.println(ip);
// }

// Setup MDNS and pass the callback function as an argument
void setup_mdns(MDNS& mdns, void (*callback)(const char*, IPAddress)) {  
  // Initialize the mDNS library
  mdns.begin(WiFi.localIP(), "Remote");

  // Set the callback function for when the name is resolved
  mdns.setNameResolvedCallback(callback);
}

// MDNS loop to resolve the hostname
void loop_mdns(MDNS& mdns, const char* hostname) {   
  // Check if mDNS is already resolving a host name
  if (!mdns.isResolvingName()) {
    Serial.print("Resolving '");
    Serial.print(hostname);
    Serial.println("' via Multicast DNS (Bonjour)...");
    
    // Resolve the host name via mDNS
    mdns.resolveName(hostname, TIMEOUT);
  }  

  // Run the mDNS process
  mdns.run();
}
