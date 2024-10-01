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


SRAM sram(4, SRAM_1024);

bool connectedWiFi  = false;
int status = WL_IDLE_STATUS;

bool newMsg = false;
static char *messageD;
uint8_t msgLen = 0;
const byte bufferSize = 64;  // Maximum buffer size
char inputBuffer[bufferSize];  // Buffer for storing data

void callback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];

  sram.seek(1);
  newMsg = true;

  memset(inputBuffer, 0, sizeof inputBuffer);
  memcpy(messageD, payload, length);
  message[length] = 0;
  memcpy(inputBuffer, message, length);
  Serial.print("[C1]["); Serial.print(inputBuffer); Serial.println("]");
  messageD = message;
  newMsg = true;

  rp2040.fifo.push(reinterpret_cast<uint32_t>(inputBuffer));

  // do something with the message
  for(uint8_t i=0; i<length; i++) {
    Serial.write(sram.read());
  }
  Serial.println();

  // Reset position for the next message to be stored
  sram.seek(1);
}

WiFiClient w0Client;
PubSubClient client(MQTT_HOST, MQTT_PORT, callback, w0Client, sram);

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("Connected to SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("Local IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

bool connectToWifi()
{
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");

    // don't continue
    while (true);
  }

  Serial.print(F("Connecting to SSID: "));
  Serial.println(WIFI_SSID);

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
    Serial.println("Resetting. Can't connect to any WiFi");

    NVIC_SystemReset();
  }

  printWifiStatus();

  connectedWiFi = (status == WL_CONNECTED);

  return (status == WL_CONNECTED);
}

bool isWiFiConnected()
{
  // You can change longer or shorter depending on your network response
  // Shorter => more responsive, but more ping traffic
  static uint8_t theTTL = 10;

  // Use ping() to test TCP connections
  if (WiFi.ping(WiFi.gatewayIP(), theTTL) == theTTL)
  {
    return true;
  }

  return false;
}

void connectToMqtt()
{
  if (isWiFiConnected()){
    if (client.connect("arduinoClient")) {
      client.publish("homeassistant/rf","hello world");
      client.subscribe("homeassistant");
    }

    sram.begin();
    sram.seek(1);

    Serial.begin(9600);
  }
}

void setup_mqtt()
{ 
  Serial.begin(115200);

  while (!Serial && millis() < 5000);

  Serial.print("\nStarting FullyFeature_RP2040W on ");
  Serial.println(BOARD_NAME);

  ///////////////////////////////////

  connectToWifi();

  ///////////////////////////////////

  connectToMqtt();
}

void loop_mqtt()
{
  client.loop();
  if(newMsg){
    newMsg = false;
    rp2040.fifo.push(reinterpret_cast<uint32_t>(inputBuffer));
    client.publish("homeassistant/rf","hello world");
  }
}
