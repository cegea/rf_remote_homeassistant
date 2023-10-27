#include <AsyncMqtt_Generic.h>
#include "remote_rf_w_mqtt_client.h"


void setup() {
  setup_cc1101_tool();
}

void loop() {
  loop_cc1101_tool();
}

void setup1() {
  setup_mqtt();
}

void loop1() {
  loop_mqtt();
}
