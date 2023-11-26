#ifndef REMOTE_RF_H
#define REMOTE_RF_H

#include <ELECHOUSE_CC1101_SRC_DRV.h>

// Define constants for modulation types
#define MODULATION_2_FSK 0
#define MODULATION_GFSK 1
#define MODULATION_ASK_OOK 2
#define MODULATION_4_FSK 3
#define MODULATION_MSK 4

// Define frequency bands
#define F868 868.23
#define F433 433.92

// #define DEBUG0
// #define DEBUG


// Define the structure for remote control data
typedef struct {
    const char* id;
    float frequency;
    byte modulation;
    const char* code;
    int replays;
    int symbolDuration_usec;
} Remote_t;

class RemoteRF
{
public:

    // Class constructor
    RemoteRF(uint8_t sck, uint8_t miso, uint8_t mosi, uint8_t ss, uint8_t gdo0, uint8_t gdo2);

    /**
     * Initialize CC1101 board with default settings. You may change your preferences here.
     *
     * This function initializes the CC1101 board with default settings, allowing you to configure
     * various parameters for your specific use case. You can adjust settings such as frequency,
     * modulation, encoding, power level, and more. Make sure to read the datasheet for the CC1101
     * for more information.
     */
    void cc1101initialize(void);

    /**
     * Configure the frequency and perform debugging operations if DEBUG0 is defined.
     *
     * This function configures the given frequency and provides optional debugging
     * output if DEBUG0 is defined. Debugging output includes information about the
     * configured frequency.
     *
     * @param frequency The frequency to be configured, in GHz.
     */
    void configureFrequency(float frequency);

    /**
     * Set the modulation for the CC1101 module.
     *
     * This function sets the modulation mode for the CC1101 module. You can choose
     * from different modulation modes such as 2-FSK, GFSK, ASK/OOK, 4-FSK, and MSK.
     *
     * @param modulationMode The modulation mode to be set (0-4).
     *   0: 2-FSK
     *   1: GFSK
     *   2: ASK/OOK
     *   3: 4-FSK
     *   4: MSK
     */
    void setModulation(byte modulationMode);

    /**
     * Transmit an RF code by replaying it with a specified symbol duration.
     *
     * This function transmits an RF code by replaying it with a specified symbol duration using the CC1101 module.
     * It replays the RF code by setting GDO0 pin according to the bits in the RF code, introducing a delay for
     * the symbol duration between each symbol transmission.
     *
     * @param symbolDuration_usec Symbol duration in microseconds.
     * @param rfCode Pointer to the buffer containing the RF code to be transmitted.
     * @param codeSize Size of the RF code buffer.
     * @param numReplays Number of times to replay the RF code.
     */
    void transmitRFCode(int symbolDuration_usec, const char* rfCode, int codeSize, int numReplays);

    /**
     * Find a remote control by its ID in the specified array of remote controls.
     * 
     * This function searches for a remote control in the specified array of `Remote_t`
     * structures based on the provided `id`. If a matching remote control is found, a
     * pointer to the `Remote_t` structure is returned; otherwise, NULL is returned.
     *
     * @param id The ID of the remote control to find.
     * @param remoteControlsArray The array of remote controls to search in.
     * @param arraySize The size of the remoteControlsArray.
     * @return A pointer to the matching `Remote_t` structure or NULL if not found.
     */
    Remote_t* findRemoteControlByID(char* id, Remote_t remoteControlsArray[], size_t arraySize);

    /**
     * Process incoming commands from the FIFO using the specified array of remote controls.
     * 
     * This function processes incoming commands retrieved from the FIFO and executes them. 
     * It reads commands from the FIFO, searches for the matching remote control by ID in
     * the specified array, sets the frequency, and executes the code (if DEBUG is defined).
     *
     * @param remoteControlsArray The array of remote controls to search in.
     * @param arraySize The size of the remoteControlsArray.
     */
    void processIncomingCommands(Remote_t remoteControlsArray[], size_t arraySize);


private:

    uint8_t _sck;
    uint8_t _miso;
    uint8_t _mosi;
    uint8_t _ss; 
    uint8_t _gdo0;
    uint8_t _gdo2;

    void asciitohex(char *hex, const char *ascii,int len);
    void hextoascii(byte *ascii_ptr, byte *hex_ptr,int len);
};


#endif
