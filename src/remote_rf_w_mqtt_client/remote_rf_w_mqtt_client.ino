#include <AsyncMqtt_Generic.h>
#include "remote_rf_w_mqtt_client.h"


// Declare remotes:
Remote_t remoteControlArray[] = {
    {"CHISME_ON", F433, MODULATION_ASK_OOK, "CODE_CHISME_ON", 4, 300},
    {"CHISME_OFF", F433, MODULATION_ASK_OOK, "CODE_CHISME_OFF", 4, 300},
    {"INT_DOOR", F868, MODULATION_ASK_OOK, "CODE_INTERIOR_DOOR", 8, 215},
    {"EXT_DOOR", F868, MODULATION_ASK_OOK, "CODE_EXTERIOR_DOOR", 8, 215}
};

// Get the size of the remoteControlArray
size_t remoteControlArraySize = sizeof(remoteControlArray) / sizeof(remoteControlArray[0]);

void setup() {
  cc1101initialize(SCK, MISO, MOSI, SS, GDO0, GDO2);
}

void loop() {
  processIncomingCommands(remoteControlArray,remoteControlArraySize);
}

void setup1() {
  setup_mqtt();
}

void loop1() {
  loop_mqtt();
}
