// Multicore doc here: https://github.com/earlephilhower/arduino-pico/blob/master/docs/multicore.rst

// To be included only in main(), .ino with setup() to avoid Multiple Def
#include <AsyncMqtt_Generic.h>


void setup() {
  setup_cc1101_tool();
}

void loop() {
  loop_cc1101_tool();

}

void setup1() {
  delay(1000);
  setup_mqtt();
}

void loop1() {
  loop_mqtt();

}
