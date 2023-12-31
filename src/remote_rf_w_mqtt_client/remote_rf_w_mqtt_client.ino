#include <AsyncMqtt_Generic.h>
#include "remote_rf_w_mqtt_client.h"

//Declare remoteRF

RemoteRF remote(SCK, MISO, MOSI, SS, GDO0, GDO2);

// Declare remotes:
Remote_t remoteControlArray[] = {
    {"CHISME_ON", F433, MODULATION_ASK_OOK, CODE_CHISME_ON, 4, 300},
    {"CHISME_OFF", F433, MODULATION_ASK_OOK, CODE_CHISME_OFF, 4, 300},
    {"INT_DOOR", F868, MODULATION_ASK_OOK, CODE_INTERIOR_DOOR, 8, 215},
    {"EXT_DOOR", F868, MODULATION_ASK_OOK, CODE_EXTERIOR_DOOR, 8, 215}
};

// Get the size of the remoteControlArray
size_t remoteControlArraySize = sizeof(remoteControlArray) / sizeof(remoteControlArray[0]);

void setup1() {
  remote.cc1101initialize();
}

void loop1() {
  remote.processIncomingCommands(remoteControlArray,remoteControlArraySize);
}

void setup() {
  setup_mqtt();
}

void loop() {
  loop_mqtt();
}
