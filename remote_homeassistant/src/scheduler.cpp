#include "scheduler.h"
#include "provisioning.h"
#include "mqtt_client.h"

Ticker ticker_command_server;
Ticker ticker_mqtt;

void setup_scheduler()
{
  ticker_command_server.attach_ms(100, command_server_provisioning);

  ticker_mqtt.attach(5, MQTT_publish_time_since_init);
}