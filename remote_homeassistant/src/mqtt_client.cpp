#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <SRAM.h>
#include <string>
#include "secrets.h"
#include "RP2040.h"
#include "mdns.h"
#include "provisioning.h"

// Private function declarations

/**
 * @brief Function that gets called when the hostname is found
 * 
 * @param name 
 * @param ip 
 */
void __resolver_callback(const char* name, IPAddress ip);

/**
 * @brief Callback for the publish and the subscribe
 * 
 * @param topic 
 * @param payload 
 * @param length 
 */
void __mqtt_callback(char* topic, byte* payload, unsigned int length);

/**
 * @brief UART the WiFi status
 * 
 */
void __printWifiStatus(void);

/**
 * @brief Try to stablish WiFi connection (most likely will disappear)
 * 
 * @return true 
 * @return false 
 */
bool __connectToWifi(void);

/**
 * @brief Check WiFi connection. (Most likely will disappear)
 * 
 * @return true 
 * @return false 
 */
bool __isWiFiConnected(void);

/**
 * @brief Resolve MQTT server and connect to it.
 * 
 */
void __connectToMqtt(void);

static uint8_t theTTL = 10;

bool connectedWiFi  = false;
bool connectedMQTT  = false;
int status = WL_IDLE_STATUS;

bool newMsg = false;
static char *messageD;
uint8_t msgLen = 0;
const byte bufferSize = 64;  // Maximum buffer size
char inputBuffer[bufferSize];  // Buffer for storing data

WiFiUDP udp;
MDNS mdns(udp);

IPAddress __ip = INADDR_NONE;

WiFiClient w0Client;
PubSubClient client("host", 1234, __mqtt_callback, w0Client);

void __resolver_callback(const char* name, IPAddress ip){
  if (ip != INADDR_NONE) {
    __ip = ip;
  } else {
#ifdef DEBUG_PROVISIONING
    DEBUG_RP2040_PORT.print("Resolving '");
    DEBUG_RP2040_PORT.print(name);
    DEBUG_RP2040_PORT.println("' timed out.");
#endif
  }
}

void __mqtt_callback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];

  newMsg = true;
#ifdef DEBUG_PROVISIONING
  DEBUG_RP2040_PORT.print("\n[Core 1][Length:"); DEBUG_RP2040_PORT.print(length); DEBUG_RP2040_PORT.println("]");
#endif
  if (length > sizeof(inputBuffer)){
    client.publish("remote/error","Payload too long");
  }
  else{
    memset(inputBuffer, 0, sizeof(inputBuffer));
    memcpy(messageD, payload, length);
    message[length] = '\n';
    memcpy(inputBuffer, message, length);
#ifdef DEBUG_PROVISIONING
    DEBUG_RP2040_PORT.print("\n[Core 1][Topic:"); DEBUG_RP2040_PORT.print(topic); DEBUG_RP2040_PORT.println("]");
    DEBUG_RP2040_PORT.print("\n[");DEBUG_RP2040_PORT.print(inputBuffer); DEBUG_RP2040_PORT.println("]");
#endif
    messageD = message;
    newMsg = true;
  }


  // rp2040.fifo.push(reinterpret_cast<uint32_t>(inputBuffer));

}

void __printWifiStatus()
{
#ifdef DEBUG_PROVISIONING
  // print the SSID of the network you're attached to:
  DEBUG_RP2040_PORT.print("\nConnected to SSID: ");
  DEBUG_RP2040_PORT.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  DEBUG_RP2040_PORT.print("Local IP Address: ");
  DEBUG_RP2040_PORT.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  DEBUG_RP2040_PORT.print("Signal strength (RSSI):");
  DEBUG_RP2040_PORT.print(rssi);
  DEBUG_RP2040_PORT.println(" dBm");
#endif
}

bool __connectToWifi()
{
  wifi_settings user_wifi = {};
  user_wifi = read_EEPROM_wifi_credentials();

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    DEBUG_RP2040_PORT.println("Communication with WiFi module failed!");

    // don't continue
    while (true);
  }
#ifdef DEBUG_PROVISIONING
  DEBUG_RP2040_PORT.print(F("Connecting to SSID: "));
  DEBUG_RP2040_PORT.println(user_wifi.ssid);
#endif

#define MAX_NUM_WIFI_CONNECT_TRIES_PER_LOOP       20

  uint8_t numWiFiConnectTries = 0;

  // attempt to connect to WiFi network
  while ( (status != WL_CONNECTED) && (numWiFiConnectTries++ < MAX_NUM_WIFI_CONNECT_TRIES_PER_LOOP) )
  {
    status = WiFi.begin(user_wifi.ssid, user_wifi.password);

    delay(500);
  }

  if (status != WL_CONNECTED)
  {
    // Restart for Portenta as something is very wrong
#ifdef DEBUG_PROVISIONING    
    DEBUG_RP2040_PORT.println("Resetting. Can't connect to any WiFi");
#endif
    NVIC_SystemReset();
  }

  __printWifiStatus();

  connectedWiFi = (status == WL_CONNECTED);

  return (status == WL_CONNECTED);
}

bool __isWiFiConnected()
{
  // Use ping() to test TCP connections
#ifdef DEBUG_PROVISIONING
  DEBUG_RP2040_PORT.println("\nGateway IP: ");
  DEBUG_RP2040_PORT.print(WiFi.gatewayIP());
#endif
  if (WiFi.ping(WiFi.gatewayIP(), theTTL) == theTTL)
  {
    return true;
  }

  return false;
}

void __connectToMqtt()
{
  mqtt_settings user_mqtt = {};
  user_mqtt = read_EEPROM_mqtt_credentials();

  uint8_t retries = 0;
  const char *hostname = user_mqtt.host;

  if (__isWiFiConnected()){
  __printWifiStatus();
#ifdef DEBUG_PROVISIONING
  DEBUG_RP2040_PORT.println("\nTry to connect to MQTT. Ping to server");
#endif
  if (__ip != INADDR_NONE)
  {
    loop_mdns(mdns,hostname);
#ifdef DEBUG_PROVISIONING
    DEBUG_RP2040_PORT.print("Resolved: ");
    DEBUG_RP2040_PORT.print("\nIP of MQTT host ");
    DEBUG_RP2040_PORT.print(hostname);
    DEBUG_RP2040_PORT.print(" is ");
    DEBUG_RP2040_PORT.println(__ip);
#endif
  }  

  if (WiFi.ping(__ip, theTTL) == theTTL)
  {
#ifdef DEBUG_PROVISIONING
    DEBUG_RP2040_PORT.println("\nPing OK");
#endif
    client.setServer(__ip,user_mqtt.port);
  }
  else{
#ifdef DEBUG_PROVISIONING
    DEBUG_RP2040_PORT.println("\nPing NOT OK");
#endif
  }

    if (client.connect("remote",user_mqtt.user, user_mqtt.passwd)) {
#ifdef DEBUG_PROVISIONING
      DEBUG_RP2040_PORT.println("\nConnected to MQTT");
#endif
      client.publish("remote/status","init");
      client.subscribe("remote/payload");
      client.subscribe("remote/rf");
      connectedMQTT = true;
    }
  }
}

void MQTT_publish_time_since_init(){
  if (connectedMQTT)
  {
    client.publish("homeassistant/init_since", std::to_string(rp2040.getCycleCount64()).c_str());
  }
  
}

void setup_mqtt()
{
#ifdef DEBUG_PROVISIONING
  DEBUG_RP2040_PORT.println("\nStarting MQTT");
#endif
  setup_mdns(mdns, __resolver_callback);
}

void loop_mqtt()
{
  if (client.state() == MQTT_CONNECTED){
    client.loop();
  }
  else if (client.state() < MQTT_CONNECTED){ // Some error
    client.disconnect();
    __connectToMqtt();
  }
  else{    
    __connectToMqtt();
  }
  if(newMsg){
    newMsg = false;
    // rp2040.fifo.push(reinterpret_cast<uint32_t>(inputBuffer));
    client.publish("remote/status","looping");
  }
}
