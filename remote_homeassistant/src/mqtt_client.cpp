#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <SRAM.h>
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
    Serial1.print("Resolving '");
    Serial1.print(name);
    Serial1.println("' timed out.");
  }
}

void __mqtt_callback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];

  newMsg = true;

  Serial1.print("\n[Core 1][Length:"); Serial1.print(length); Serial1.println("]");

  if (length > sizeof(inputBuffer)){
    client.publish("remote/error","Payload too long");
  }
  else{
    memset(inputBuffer, 0, sizeof(inputBuffer));
    memcpy(messageD, payload, length);
    message[length] = '\n';
    memcpy(inputBuffer, message, length);
    Serial1.print("\n[Core 1][Topic:"); Serial1.print(topic); Serial1.println("]");
    Serial1.print("\n[");Serial1.print(inputBuffer); Serial1.println("]");
    messageD = message;
    newMsg = true;
  }


  // rp2040.fifo.push(reinterpret_cast<uint32_t>(inputBuffer));

}

void __printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial1.print("\nConnected to SSID: ");
  Serial1.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial1.print("Local IP Address: ");
  Serial1.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial1.print("Signal strength (RSSI):");
  Serial1.print(rssi);
  Serial1.println(" dBm");
}

bool __connectToWifi()
{
  wifi_settings user_wifi = {};
  user_wifi = read_EEPROM_wifi_credentials();

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial1.println("Communication with WiFi module failed!");

    // don't continue
    while (true);
  }

  Serial1.print(F("Connecting to SSID: "));
  Serial1.println(user_wifi.ssid);

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
    Serial1.println("Resetting. Can't connect to any WiFi");

    NVIC_SystemReset();
  }

  __printWifiStatus();

  connectedWiFi = (status == WL_CONNECTED);

  return (status == WL_CONNECTED);
}

bool __isWiFiConnected()
{
  // Use ping() to test TCP connections
  Serial1.println("\nGateway IP: ");
  Serial1.print(WiFi.gatewayIP());
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
  Serial1.println("\nTry to connect to MQTT. Ping to server");

  if (__ip != INADDR_NONE)
  {
    loop_mdns(mdns,hostname);
    Serial1.print("Resolved: ");
    Serial1.print("\nIP of MQTT host ");
    Serial1.print(hostname);
    Serial1.print(" is ");
    Serial1.println(__ip);
  }  

  if (WiFi.ping(__ip, theTTL) == theTTL)
  {
    Serial1.println("\nPing OK");
    client.setServer(__ip,user_mqtt.port);
  }
  else{
    Serial1.println("\nPing NOT OK");
  }

    if (client.connect("remote",user_mqtt.user, user_mqtt.passwd)) {
      Serial1.println("\nConnected to MQTT");
      client.publish("remote/status","init");
      client.subscribe("remote/payload");
      client.subscribe("remote/rf");
      connectedMQTT = true;
    }
  }
}

void setup_mqtt()
{
  Serial1.println("\nStarting MQTT");
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
