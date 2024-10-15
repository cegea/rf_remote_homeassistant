#include "remote_rf.h"
#include <SPI.h>
#include "ELECHOUSE_CC1101_SRC_DRV.h"

// Portions of this code are based on the cc1101-tool repository by mcore1976.
// Repository: https://github.com/mcore1976/cc1101-tool

RemoteRF::RemoteRF(uint8_t sck, uint8_t miso, uint8_t mosi, uint8_t ss, uint8_t gdo0, uint8_t gdo2){
    _sck =  sck;
    _miso = miso;
    _mosi = mosi;
    _ss =  ss;
    _gdo0 = gdo0;
    _gdo2 = gdo2;
}

void RemoteRF::cc1101initialize() {
    // initializing library with custom pins selected

    SPI.setRX(_miso);
    SPI.setTX(_mosi);
    SPI.setSCK(_sck);
    SPI.setCS(_ss);

    gpio_init(_gdo0);
    gpio_set_dir(_gdo0, GPIO_OUT);

    // SPI.beginTransaction(spisettings);
    ELECHOUSE_cc1101.setSpiPin(_sck, _miso, _mosi, _ss);
    ELECHOUSE_cc1101.setGDO(_gdo0, _gdo2);

    if (ELECHOUSE_cc1101.getCC1101()) {  // Check the CC1101 Spi connection.
      DEBUG_APPLICATION_PORT.println("[C1] SPI Connection OK");
    } else {
      DEBUG_APPLICATION_PORT.println("[C1] SPI Connection NOT OK");
    }

    // Main part to tune CC1101 with proper frequency, modulation and encoding    
    ELECHOUSE_cc1101.Init();                // must be set to initialize the cc1101!
    ELECHOUSE_cc1101.setGDO0(_gdo0);         // set lib internal gdo pin (gdo0). Gdo2 not use for this example.
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

void RemoteRF::configureFrequency(float frequency) {
  // Configure the frequency
  ELECHOUSE_cc1101.setMHZ(frequency);
  
  #ifdef DEBUG0
    DEBUG_APPLICATION_PORT.print("Configuring frequency: ");
    DEBUG_APPLICATION_PORT.print(frequency, 2); // Print the frequency with 2 decimal places
    DEBUG_APPLICATION_PORT.println(" MHz");
  #endif
}

void RemoteRF::setModulation(byte modulationMode) {
  ELECHOUSE_cc1101.setModulation(modulationMode);
  
  #ifdef DEBUG0
    DEBUG_APPLICATION_PORT.print("Setting modulation mode: ");
    DEBUG_APPLICATION_PORT.println(modulationMode);
  #endif
}


void RemoteRF::transmitRFCode(int symbolDuration_usec, const char* rfCode, int codeSize, int numReplays) {

  char buffer[codeSize];

  // Setup asynchronous mode on CC1101 and go into TX mode with GDO0 pin processing
  ELECHOUSE_cc1101.setCCMode(0);
  ELECHOUSE_cc1101.setPktFormat(3);
  ELECHOUSE_cc1101.SetTx();

  hextoascii((byte *)buffer, (byte *)rfCode, strlen(rfCode));  
  
  #ifdef DEBUG0
    DEBUG_APPLICATION_PORT.print(F("\r\nTransmitting RF code: "));
    for (int i = 0; i < codeSize/2; i++) {
      DEBUG_APPLICATION_PORT.print(buffer[i], HEX);
      DEBUG_APPLICATION_PORT.print("");
    }
    DEBUG_APPLICATION_PORT.println();
  #endif

  pinMode(_gdo0, OUTPUT);

  for (int replay = 0; replay < numReplays; replay++) {
    for (int i = 0; i < codeSize/2; i++) {
      byte codeByte = buffer[i];
      for (int j = 7; j >= 0; j--) {
        digitalWrite(_gdo0, bitRead(codeByte, j)); // Set GDO0 according to the bits in the RF code
        delayMicroseconds(symbolDuration_usec); // Delay for the specified symbol duration
      }
    }
  }

  #ifdef DEBUG0
    DEBUG_APPLICATION_PORT.print(F("\r\nTransmitting RF code complete.\r\n\r\n"));
  #endif

  // Set normal packet format again
  ELECHOUSE_cc1101.setCCMode(1);
  ELECHOUSE_cc1101.setPktFormat(0);
  ELECHOUSE_cc1101.SetTx();
  // pinMode(gdo0pin, INPUT);

  #ifdef DEBUG0
    DEBUG_APPLICATION_PORT.print(F("\r\nTransmitting RF code complete.\r\n\r\n"));
  #endif

  ELECHOUSE_cc1101.goSleep();
  // pinMode(gdo0pin, INPUT);
}

Remote_t* RemoteRF::findRemoteControlByID(char* id, Remote_t remoteControlsArray[], size_t arraySize) {
    for (size_t i = 0; i < arraySize; i++) {
        if (strcmp(id, remoteControlsArray[i].id) == 0) {
            return &remoteControlsArray[i];
        }
    }
    return NULL; // Remote control not found
}

void RemoteRF::processIncomingCommands(Remote_t remoteControlsArray[], size_t arraySize) {
  // Check for available data in the FIFO
  int availableFifo = rp2040.fifo.available();
  
  while (availableFifo > 0) {
    #ifdef DEBUG
    DEBUG_APPLICATION_PORT.print("Ready data from MQTT: ");
    DEBUG_APPLICATION_PORT.println(availableFifo);
    #endif
    
    // Retrieve the command from the FIFO
    char* id = reinterpret_cast<char*>(rp2040.fifo.pop());
    
    // Search for the matching remote control by ID
    Remote_t* remote = findRemoteControlByID(id, remoteControlsArray, arraySize);
    
    if (remote) {
      // Set the frequency
      configureFrequency(remote->frequency); // Implement this function

      // Set modulation
      setModulation(remote->modulation);
      
      #ifdef DEBUG
      DEBUG_APPLICATION_PORT.printf("MQTT data: %s\n", id);
      DEBUG_APPLICATION_PORT.print("[C1]["); 
      DEBUG_APPLICATION_PORT.print(id);
      DEBUG_APPLICATION_PORT.println("]");
      #endif
      
      // Execute the code with the specified symbol duration
      transmitRFCode(remote->symbolDuration_usec, remote->code, strlen(remote->code), remote->replays);
    } else {
      #ifdef DEBUG
      DEBUG_APPLICATION_PORT.println("Remote control not found.");
      #endif
    }
  }
}

void RemoteRF::processIncomingCommands() {
  // Check for available data in the FIFO
  int availableFifo = rp2040.fifo.available();
  mutex_enter_blocking(&remoteDataMutex);
  DEBUG_APPLICATION_PORT.print("No data");
  
  while (availableFifo > 0) {
    #ifdef DEBUG_RADIO
    DEBUG_APPLICATION_PORT.print("Ready data from MQTT: ");
    DEBUG_APPLICATION_PORT.println(availableFifo);
    #endif
    
    // Delete flag
    rp2040.fifo.pop();
    
    // Set the frequency
    configureFrequency(remoteControl.frequency); // Implement this function

    // Set modulation
    setModulation(remoteControl.modulation);
    
    #ifdef DEBUG_RADIO
    // DEBUG_APPLICATION_PORT.printf("MQTT data: %s\n", id);
    DEBUG_APPLICATION_PORT.print("[C1]["); 
    DEBUG_APPLICATION_PORT.print(remoteControl.id);
    DEBUG_APPLICATION_PORT.println("]");
    #endif
    
    // Execute the code with the specified symbol duration
    transmitRFCode(remoteControl.symbolDuration_usec, remoteControl.code, strlen(remoteControl.code), remoteControl.replays);

    mutex_exit(&remoteDataMutex); 
  }
}

// // convert char table to string with hex numbers
// void RemoteRF::asciitohex(byte *ascii_ptr, byte *hex_ptr,int len)
// {
//     byte i,j,k;
//     for(i = 0; i < len; i++)
//     {
//       // high byte first
//       j = ascii_ptr[i] / 16;
//       if (j>9) 
//          { k = j - 10 + 65; }
//       else 
//          { k = j + 48; }
//       hex_ptr[2*i] = k ;
//       // low byte second
//       j = ascii_ptr[i] % 16;
//       if (j>9) 
//          { k = j - 10 + 65; }
//       else
//          { k = j + 48; }
//       hex_ptr[(2*i)+1] = k ; 
//     };
//     hex_ptr[(2*i)+2] = '\0' ; 
// }


// // convert string with hex numbers to array of bytes
// void RemoteRF::hextoascii(char *ascii_ptr, const char *hex_ptr,int len)
// {
//     char i,j;
//     for(i = 0; i < (len/2); i++)
//      { 
//      j = hex_ptr[i*2];
//      if ((j>47) && (j<58))  ascii_ptr[i] = (j - 48) * 16;
//      if ((j>64) && (j<71))  ascii_ptr[i] = (j - 55) * 16;
//      if ((j>96) && (j<103)) ascii_ptr[i] = (j - 87) * 16;
//      j = hex_ptr[i*2+1];
//      if ((j>47) && (j<58))  ascii_ptr[i] = ascii_ptr[i]  + (j - 48);
//      if ((j>64) && (j<71))  ascii_ptr[i] = ascii_ptr[i]  + (j - 55);
//      if ((j>96) && (j<103)) ascii_ptr[i] = ascii_ptr[i]  + (j - 87);
//      };
//     ascii_ptr[i++] = '\0' ;
// }

void RemoteRF::asciitohex(char *hex, const char *ascii,int len)
{
  char value;
  for (size_t i = 0; i < sizeof(ascii); i++){
    value = ascii[i];
    if (value < 'a'){
			value = value-48;
    }
		else{
			value = value-87;
    }
    hex[i] = value;
  }
}

// convert string with hex numbers to array of bytes
void RemoteRF::hextoascii(byte *ascii_ptr, byte *hex_ptr,int len)
{
    byte i,j;
    for(i = 0; i < (len/2); i++)
     { 
     j = hex_ptr[i*2];
     if ((j>47) && (j<58))  ascii_ptr[i] = (j - 48) * 16;
     if ((j>64) && (j<71))  ascii_ptr[i] = (j - 55) * 16;
     if ((j>96) && (j<103)) ascii_ptr[i] = (j - 87) * 16;
     j = hex_ptr[i*2+1];
     if ((j>47) && (j<58))  ascii_ptr[i] = ascii_ptr[i]  + (j - 48);
     if ((j>64) && (j<71))  ascii_ptr[i] = ascii_ptr[i]  + (j - 55);
     if ((j>96) && (j<103)) ascii_ptr[i] = ascii_ptr[i]  + (j - 87);
     };
    ascii_ptr[i++] = '\0' ;
}


