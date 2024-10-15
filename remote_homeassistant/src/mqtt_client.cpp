#include <WiFi.h>
#include <PubSubClient.h>
#include <SRAM.h>
#include <string>
#include <ArduinoJson.h>
#include "mqtt_client.h"
#include "RP2040.h"
#include "mdns.h"
#include "provisioning.h"
#include "remote_rf.h"

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
char inputBuffer[MQTT_MAX_PACKET_SIZE] PSRAM;  // Buffer for storing data

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
    DEBUG_APPLICATION_PORT.print("Resolving '");
    DEBUG_APPLICATION_PORT.print(name);
    DEBUG_APPLICATION_PORT.println("' timed out.");
#endif
  }
}

void __mqtt_callback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];
  char *messageD;
  uint8_t msgLen = 0;
  newMsg = true;
#ifdef DEBUG_MQTT
  DEBUG_APPLICATION_PORT.print("\n[Core 1][Length:"); DEBUG_APPLICATION_PORT.print(length); DEBUG_APPLICATION_PORT.println("]");
#endif
  if (length > sizeof(inputBuffer)){
    client.publish("remote/error","Payload too long");
  }
  else{
    bzero(inputBuffer, sizeof(inputBuffer));
    rp2040.memcpyDMA(inputBuffer, payload, length);
#ifdef DEBUG_MQTT
    DEBUG_APPLICATION_PORT.print("\n[Core 1][Topic:"); DEBUG_APPLICATION_PORT.print(topic); DEBUG_APPLICATION_PORT.println("]");
    DEBUG_APPLICATION_PORT.print("\n[");DEBUG_APPLICATION_PORT.print(inputBuffer); DEBUG_APPLICATION_PORT.println("]");
#endif
    MQTT_command_server(topic);
    messageD = message;
    newMsg = true;
  }


  // rp2040.fifo.push(reinterpret_cast<uint32_t>(inputBuffer));

}

void __printWifiStatus()
{
#ifdef DEBUG_PROVISIONING
  // print the SSID of the network you're attached to:
  DEBUG_APPLICATION_PORT.print("\nConnected to SSID: ");
  DEBUG_APPLICATION_PORT.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  DEBUG_APPLICATION_PORT.print("Local IP Address: ");
  DEBUG_APPLICATION_PORT.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  DEBUG_APPLICATION_PORT.print("Signal strength (RSSI):");
  DEBUG_APPLICATION_PORT.print(rssi);
  DEBUG_APPLICATION_PORT.println(" dBm");
#endif
}

bool __connectToWifi()
{
  wifi_settings user_wifi = {};
  user_wifi = read_EEPROM_wifi_credentials();

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
#ifdef DEBUG_MQTT
    DEBUG_APPLICATION_PORT.println("Communication with WiFi module failed!");
#endif
    // don't continue
    while (true);
  }
#ifdef DEBUG_PROVISIONING
  DEBUG_APPLICATION_PORT.print(F("Connecting to SSID: "));
  DEBUG_APPLICATION_PORT.println(user_wifi.ssid);
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
    DEBUG_APPLICATION_PORT.println("Resetting. Can't connect to any WiFi");
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
  DEBUG_APPLICATION_PORT.println("\nGateway IP: ");
  DEBUG_APPLICATION_PORT.print(WiFi.gatewayIP());
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
  DEBUG_APPLICATION_PORT.println("\nTry to connect to MQTT. Ping to server");
#endif
  if (__ip != INADDR_NONE)
  {
    loop_mdns(mdns,hostname);
#ifdef DEBUG_PROVISIONING
    DEBUG_APPLICATION_PORT.print("Resolved: ");
    DEBUG_APPLICATION_PORT.print("\nIP of MQTT host ");
    DEBUG_APPLICATION_PORT.print(hostname);
    DEBUG_APPLICATION_PORT.print(" is ");
    DEBUG_APPLICATION_PORT.println(__ip);
#endif
  }  

  if (WiFi.ping(__ip, theTTL) == theTTL)
  {
#ifdef DEBUG_PROVISIONING
    DEBUG_APPLICATION_PORT.println("\nPing OK");
#endif
    client.setServer(__ip,user_mqtt.port);
  }
  else{
#ifdef DEBUG_PROVISIONING
    DEBUG_APPLICATION_PORT.println("\nPing NOT OK");
#endif
  }

    if (client.connect("remote",user_mqtt.user, user_mqtt.passwd)) {
#ifdef DEBUG_PROVISIONING
      DEBUG_APPLICATION_PORT.println("\nConnected to MQTT");
#endif
      client.publish(TOPIC_STATUS,"init");
      client.subscribe(TOPIC_TEST_DOWN);
      client.subscribe(TOPIC_COMMAND_DELETE_CREDENTIALS);
      client.subscribe(TOPIC_COMMAND_REBOOT);
      client.subscribe(TOPIC_RF);
      connectedMQTT = true;
    }
  }
}

void MQTT_publish_time_since_init(){
  if (connectedMQTT)
  {
    client.publish("remote/init_since", 
      std::to_string(rp2040.getCycleCount64()).c_str());
  }
  
}

void MQTT_command_server(char* topic){

  if (strcmp(topic, TOPIC_TEST_DOWN) == 0)
  {
    client.publish(TOPIC_TEST_UP, inputBuffer);
  }
  else if (strcmp(topic, TOPIC_COMMAND_REBOOT) == 0)
  {
    client.publish(TOPIC_STATUS, "rebooting");
    rp2040.reboot();
  }
  else if (strcmp(topic, TOPIC_COMMAND_DELETE_CREDENTIALS) == 0)
  {
    provisioning_delete_credentials();
  }
  else if (strcmp(topic, TOPIC_RF) == 0)
  {
    client.publish(TOPIC_STATUS, "Data received");

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, inputBuffer);

    if (!error) {         
      // Extract values from JSON
      // mutex_enter_blocking(&remoteDataMutex);
      remoteControl.frequency = doc["freq"].as<float>();
      remoteControl.id = doc["id"].as<const char*>();
      remoteControl.replays = doc["replays"].as<int>();
      remoteControl.code = doc["payload"].as<const char*>();
      remoteControl.symbolDuration_usec = doc["symbol_duration"].as<int>();
      remoteControl.modulation = doc["modulation"].as<byte>();
#ifdef DEBUG_MQTT
      // Now you can use these variables in your program
      DEBUG_APPLICATION_PORT.println("RF Frequency: " + String(remoteControl.frequency, 2));
      DEBUG_APPLICATION_PORT.println("RF ID: " + String(remoteControl.id));
      DEBUG_APPLICATION_PORT.println("RF Replays: " + String(remoteControl.replays));
      DEBUG_APPLICATION_PORT.println("RF Payload: " + String(remoteControl.code));
      DEBUG_APPLICATION_PORT.println("RF Symbol Duration: " + String(remoteControl.symbolDuration_usec));
      DEBUG_APPLICATION_PORT.println("RF Modulation: " + String(remoteControl.modulation));
#endif      
      // mutex_exit(&remoteDataMutex); 

      rp2040.fifo.push(255);

      client.publish(TOPIC_STATUS, "Data received and pushed to Core1");
    } else {
        client.publish(TOPIC_STATUS, "Failed to parse JSON");
    }
  }
  else
  {
    client.publish(TOPIC_ERROR_UNKNOWN_CMD, topic);
  }  
}

void setup_mqtt()
{
#ifdef DEBUG_PROVISIONING
  DEBUG_APPLICATION_PORT.println("\nStarting MQTT");
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
    // client.publish(TOPIC_STATUS,"looping");
  }
}
