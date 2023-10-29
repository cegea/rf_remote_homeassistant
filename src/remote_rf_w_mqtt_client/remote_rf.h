#ifndef REMOTE_RF_H
#define REMOTE_RF_H

#include <ELECHOUSE_CC1101_SRC_DRV.h>

// Define the structure for remote control data
typedef struct {
    const char* id;
    float frequency;
    byte modulation;
    const char* code;
    int replays;
    int symbolDuration_usec;
} Remote_t;

// Function prototypes
void cc1101initialize(uint8_t sck, uint8_t miso, uint8_t mosi, uint8_t ss, uint8_t gdo0, uint8_t gdo2);
void configureFrequency(float frequency);
void setModulation(byte modulationMode);
void transmitRFCode(int symbolDuration_usec, char* rfCode, int codeSize, int numReplays);
Remote_t* findRemoteControlByID(const char* id, Remote_t remoteControlsArray[], size_t arraySize);
void processIncomingCommands(Remote_t remoteControlsArray[], size_t arraySize);

#endif
