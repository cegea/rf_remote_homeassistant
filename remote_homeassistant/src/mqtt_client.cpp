/*
 Example of using a Stream object to store the message payload

 Uses SRAM library: https://github.com/ennui2342/arduino-sram
 but could use any Stream based class such as SD

  - connects to an MQTT server
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"
*/

#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <SRAM.h>
#include "secrets.h"
#include "RP2040.h"
#include "mdns.h"

  // You can change longer or shorter depending on your network response
  // Shorter => more responsive, but more ping traffic
  static uint8_t theTTL = 10;

SRAM sram(4, SRAM_1024);

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


void resolver_callback(const char* name, IPAddress ip){
  if (ip != INADDR_NONE) {
    Serial1.print("The IP address for '");
    Serial1.print(name);
    Serial1.print("' is ");
    Serial1.println(ip);
    __ip = ip;
  } else {
    Serial1.print("Resolving '");
    Serial1.print(name);
    Serial1.println("' timed out.");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];

  sram.seek(1);
  newMsg = true;

  memset(inputBuffer, 0, sizeof inputBuffer);
  memcpy(messageD, payload, length);
  message[length] = 0;
  memcpy(inputBuffer, message, length);
  Serial1.print("[C1]["); Serial1.print(inputBuffer); Serial1.println("]");
  messageD = message;
  newMsg = true;

  rp2040.fifo.push(reinterpret_cast<uint32_t>(inputBuffer));

  // do something with the message
  for(uint8_t i=0; i<length; i++) {
    Serial1.write(sram.read());
  }
  Serial1.println();

  // Reset position for the next message to be stored
  sram.seek(1);
}

WiFiClient w0Client;
PubSubClient client(MQTT_HOST, MQTT_PORT, callback, w0Client, sram);

void printWifiStatus()
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

bool connectToWifi()
{
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial1.println("Communication with WiFi module failed!");

    // don't continue
    while (true);
  }

  Serial1.print(F("Connecting to SSID: "));
  Serial1.println(WIFI_SSID);

#define MAX_NUM_WIFI_CONNECT_TRIES_PER_LOOP       20

  uint8_t numWiFiConnectTries = 0;

  // attempt to connect to WiFi network
  while ( (status != WL_CONNECTED) && (numWiFiConnectTries++ < MAX_NUM_WIFI_CONNECT_TRIES_PER_LOOP) )
  {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    delay(500);
  }

  if (status != WL_CONNECTED)
  {
    // Restart for Portenta as something is very wrong
    Serial1.println("Resetting. Can't connect to any WiFi");

    NVIC_SystemReset();
  }

  printWifiStatus();

  connectedWiFi = (status == WL_CONNECTED);

  return (status == WL_CONNECTED);
}

bool isWiFiConnected()
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

void connectToMqtt()
{
  uint8_t retries = 0;
  const char *hostname = "homeassistant";

  if (isWiFiConnected()){
  printWifiStatus();
  Serial1.println("\nTry to connect to MQTT. Ping to server");

  loop_mdns(mdns,hostname);

  if (__ip != INADDR_NONE)
  {
    Serial1.print("Resolved: ");
    Serial1.println(__ip);
  }
  else
  {
    Serial1.println("Not resolved");
  }
  // while(WiFi.hostByName(hostname, __ip) != 1){
  //   if(retries++>200){
  //     break;
  //   }
  // }
  // WiFi.hostByName(MQTT_HOST, __ip);
  Serial1.print("\nIP of MQTT host ");
  Serial1.print(hostname);
  Serial1.print(" is ");
  Serial1.println(__ip);

  if (WiFi.ping(__ip, theTTL) == theTTL)
  {
    Serial1.println("\nPing OK");
    client.setServer(__ip,MQTT_PORT);
    // return true;
  }
  else{
    Serial1.println("\nPing NOT OK");
    Serial1.println(WiFi.ping(__ip, theTTL));
  }

    if (client.connect("remote",MQTT_USER, MQTT_CREDENTIALS)) {
      Serial1.println("\nConnected to MQTT");
      client.publish("homeassistant/rf","hello world");
      client.subscribe("homeassistant");
      connectedMQTT = true;
    }

    // sram.begin();
    // sram.seek(1);

  }
}

void setup_mqtt()
{ 
  // Serial1.begin(115200);

  // while (!Serial1 && millis() < 5000);

  Serial1.print("\nStarting MQTT");

  setup_mdns(mdns, resolver_callback);

  ///////////////////////////////////
  // To get here we should be connected to Wifi
  // connectToWifi();

  ///////////////////////////////////

}

void loop_mqtt()
{
  // Serial1.print("\nLooping MQTT");

  if (connectedMQTT){
    client.loop();
  }
  else{
    
    connectToMqtt();
  }
  if(newMsg){
    newMsg = false;
    rp2040.fifo.push(reinterpret_cast<uint32_t>(inputBuffer));
    client.publish("homeassistant/rf","looping");
  }
}
