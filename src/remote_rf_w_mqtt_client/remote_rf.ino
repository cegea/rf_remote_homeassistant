#include <ELECHOUSE_CC1101_SRC_DRV.h>

// Portions of this code are based on the cc1101-tool repository by mcore1976.
// Repository: https://github.com/mcore1976/cc1101-tool

/**
 * Initialize CC1101 board with default settings. You may change your preferences here.
 *
 * This function initializes the CC1101 board with default settings, allowing you to configure
 * various parameters for your specific use case. You can adjust settings such as frequency,
 * modulation, encoding, power level, and more. Make sure to read the datasheet for the CC1101
 * for more information.
 */
static void cc1101initialize(void)
{
    // initializing library with custom pins selected
     ELECHOUSE_cc1101.setSpiPin(SCK, MISO, MOSI, SS);
     ELECHOUSE_cc1101.setGDO(GDO0, GDO2);

    // Main part to tune CC1101 with proper frequency, modulation and encoding    
    ELECHOUSE_cc1101.Init();                // must be set to initialize the cc1101!
    ELECHOUSE_cc1101.setGDO0(gdo0);         // set lib internal gdo pin (gdo0). Gdo2 not use for this example.
    ELECHOUSE_cc1101.setCCMode(1);          // set config for internal transmission mode. value 0 is for RAW recording/replaying
    ELECHOUSE_cc1101.setModulation(2);      // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
    ELECHOUSE_cc1101.setMHZ(433.92);        // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
    ELECHOUSE_cc1101.setDeviation(47.60);   // Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
    ELECHOUSE_cc1101.setChannel(0);         // Set the Channelnumber from 0 to 255. Default is cahnnel 0.
    ELECHOUSE_cc1101.setChsp(199.95);       // The channel spacing is multiplied by the channel number CHAN and added to the base frequency in kHz. Value from 25.39 to 405.45. Default is 199.95 kHz.
    ELECHOUSE_cc1101.setRxBW(812.50);       // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
    ELECHOUSE_cc1101.setDRate(9.6);         // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
    ELECHOUSE_cc1101.setPA(10);             // Set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12) Default is max!
    ELECHOUSE_cc1101.setSyncMode(2);        // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
    ELECHOUSE_cc1101.setSyncWord(211, 145); // Set sync word. Must be the same for the transmitter and receiver. Default is 211,145 (Syncword high, Syncword low)
    ELECHOUSE_cc1101.setAdrChk(0);          // Controls address check configuration of received packages. 0 = No address check. 1 = Address check, no broadcast. 2 = Address check and 0 (0x00) broadcast. 3 = Address check and 0 (0x00) and 255 (0xFF) broadcast.
    ELECHOUSE_cc1101.setAddr(0);            // Address used for packet filtration. Optional broadcast addresses are 0 (0x00) and 255 (0xFF).
    ELECHOUSE_cc1101.setWhiteData(0);       // Turn data whitening on / off. 0 = Whitening off. 1 = Whitening on.
    ELECHOUSE_cc1101.setPktFormat(0);       // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins. 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX. 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
    ELECHOUSE_cc1101.setLengthConfig(1);    // 0 = Fixed packet length mode. 1 = Variable packet length mode. 2 = Infinite packet length mode. 3 = Reserved
    ELECHOUSE_cc1101.setPacketLength(0);    // Indicates the packet length when fixed packet length mode is enabled. If variable packet length mode is used, this value indicates the maximum packet length allowed.
    ELECHOUSE_cc1101.setCrc(0);             // 1 = CRC calculation in TX and CRC check in RX enabled. 0 = CRC disabled for TX and RX.
    ELECHOUSE_cc1101.setCRC_AF(0);          // Enable automatic flush of RX FIFO when CRC is not OK. This requires that only one packet is in the RXIFIFO and that packet length is limited to the RX FIFO size.
    ELECHOUSE_cc1101.setDcFilterOff(0);     // Disable digital DC blocking filter before demodulator. Only for data rates ≤ 250 kBaud The recommended IF frequency changes when the DC blocking is disabled. 1 = Disable (current optimized). 0 = Enable (better sensitivity).
    ELECHOUSE_cc1101.setManchester(0);      // Enables Manchester encoding/decoding. 0 = Disable. 1 = Enable.
    ELECHOUSE_cc1101.setFEC(0);             // Enable Forward Error Correction (FEC) with interleaving for packet payload (Only supported for fixed packet length mode. 0 = Disable. 1 = Enable.
    ELECHOUSE_cc1101.setPRE(0);             // Sets the minimum number of preamble bytes to be transmitted. Values: 0 : 2, 1 : 3, 2 : 4, 3 : 6, 4 : 8, 5 : 12, 6 : 16, 7 : 24
    ELECHOUSE_cc1101.setPQT(0);             // Preamble quality estimator threshold. The preamble quality estimator increases an internal counter by one each time a bit is received that is different from the previous bit, and decreases the counter by 8 each time a bit is received that is the same as the last bit. A threshold of 4∙PQT for this counter is used to gate sync word detection. When PQT=0 a sync word is always accepted.
    ELECHOUSE_cc1101.setAppendStatus(0);    // When enabled, two status bytes will be appended to the payload of the packet. The status bytes contain RSSI and LQI values, as well as CRC OK.
}

/**
 * Configure the frequency and perform debugging operations if DEBUG0 is defined.
 *
 * This function configures the given frequency and provides optional debugging
 * output if DEBUG0 is defined. Debugging output includes information about the
 * configured frequency.
 *
 * @param frequency The frequency to be configured, in GHz.
 */
void configureFrequency(float frequency) {
  // Configure the frequency
  ELECHOUSE_cc1101.setMHZ(frequency);
  
  #ifdef DEBUG0
    Serial.print("Configuring frequency: ");
    Serial.print(frequency, 2); // Print the frequency with 2 decimal places
    Serial.println(" GHz");
  #endif
}

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
void setModulation(byte modulationMode) {
  ELECHOUSE_cc1101.setModulation(modulationMode);
  
  #ifdef DEBUG0
    Serial.print("Setting modulation mode: ");
    Serial.println(modulationMode);
  #endif
}

/**
 * Transmit an RF code by playing it with a specified sampling interval.
 *
 * This function transmits an RF code by playing it from a buffer with bit-banging and asynchronous
 * mode using the CC1101 module. It replays the RF code by setting GDO0 pin according to the bits
 * in the buffer and introduces a delay for the specified sampling interval.
 *
 * @param setting Sampling interval in microseconds.
 * @param rfCode Pointer to the buffer containing the RF code to be transmitted.
 * @param codeSize Size of the RF code buffer.
 */
void transmitRFCode(int setting, char* rfCode, int codeSize) {
  // Setup asynchronous mode on CC1101 and go into TX mode with GDO0 pin processing
  ELECHOUSE_cc1101.setCCMode(0);
  ELECHOUSE_cc1101.setPktFormat(3);
  ELECHOUSE_cc1101.SetTx();

  #ifdef DEBUG0
    Serial.print(F("\r\nTransmitting RF code: "));
    for (int i = 0; i < codeSize; i++) {
      Serial.print(rfCode[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  #endif

  pinMode(gdo0, OUTPUT);

  // Blink LED RX - only for Arduino Pro Micro
  digitalWrite(RXLED, LOW); // Set the RX LED ON

  for (int i = 0; i < codeSize; i++) {
    byte codeByte = rfCode[i];
    for (int j = 7; j >= 0; j--) {
      digitalWrite(gdo0, bitRead(codeByte, j)); // Set GDO0 according to the bits in the RF code
      delayMicroseconds(setting); // Delay for the selected sampling interval
    }
  }

  // Blink LED RX - only for Arduino Pro Micro
  digitalWrite(RXLED, HIGH); // Set the RX LED OFF

  #ifdef DEBUG0
    Serial.print(F("\r\nTransmitting RF code complete.\r\n\r\n"));
  #endif

  // Set normal packet format again
  ELECHOUSE_cc1101.setCCMode(1);
  ELECHOUSE_cc1101.setPktFormat(0);
  ELECHOUSE_cc1101.SetTx();
  // pinMode(gdo0pin, INPUT);
}

// #include <pio.h>

// // Define a PIO state machine instance for bit-banging
// PIO pio;
// uint sm;

// // Define a PIO program to bit-bang the RF code
// const uint32_t pio_program[] = {
//     0x0, // Instruction 0: Pull `sm[1]` low
//     0x1, // Instruction 1: Pull `sm[1]` high
//     0x2, // Instruction 2: Jump to instruction 0
// };

// /**
//  * Transmit an RF code using PIO bit-banging.
//  *
//  * This function transmits an RF code by bit-banging it using the PIO state machine.
//  *
//  * @param setting Sampling interval in microseconds.
//  * @param rfCode Pointer to the buffer containing the RF code to be transmitted.
//  * @param codeSize Size of the RF code buffer.
//  */
// void transmitRFCode(int setting, char* rfCode, int codeSize) {
//     // Setup PIO state machine for bit-banging
//     pio_sm_claim(pio, pio_claim_unused_sm(pio, true));
//     sm = pio_get_claimed_sm(pio);

//     // Load PIO program
//     pio_program_init(pio, sm, sizeof(pio_program) / sizeof(pio_program[0]), pio_program, 0, 0);

//     // Setup asynchronous mode on CC1101 and go into TX mode
//     ELECHOUSE_cc1101.setCCMode(0);
//     ELECHOUSE_cc1101.setPktFormat(3);
//     ELECHOUSE_cc1101.SetTx();

//     #ifdef DEBUG0
//         Serial.print(F("\r\nTransmitting RF code: "));
//         for (int i = 0; i < codeSize; i++) {
//             Serial.print(rfCode[i], HEX);
//             Serial.print(" ");
//         }
//         Serial.println();
//     #endif

//     pinMode(gdo0, OUTPUT);

//     // Blink LED RX - only for Arduino Pro Micro
//     digitalWrite(RXLED, LOW); // Set the RX LED ON

//     for (int i = 0; i < codeSize; i++) {
//         byte codeByte = rfCode[i];
//         for (int j = 7; j >= 0; j--) {
//             if (bitRead(codeByte, j)) {
//                 // Set `sm[1]` high
//                 pio_sm_exec(pio, sm, pio_encode_set(pio_pins, 1, 1));
//             } else {
//                 // Set `sm[1]` low
//                 pio_sm_exec(pio, sm, pio_encode_set(pio_pins, 1, 0));
//             }
//             delayMicroseconds(setting); // Delay for the selected sampling interval
//         }
//     }

//     // Blink LED RX - only for Arduino Pro Micro
//     digitalWrite(RXLED, HIGH); // Set the RX LED OFF

//     #ifdef DEBUG0
//         Serial.print(F("\r\nTransmitting RF code complete.\r\n\r\n"));
//     #endif

//     // Set normal packet format again
//     ELECHOUSE_cc1101.setCCMode(1);
//     ELECHOUSE_cc1101.setPktFormat(0);
//     ELECHOUSE_cc1101.SetTx();

//     // Release the PIO state machine
//     pio_sm_unclaim(pio, sm);
// }

