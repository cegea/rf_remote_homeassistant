# Remote RF for HomeAssistant
RF remote based on RP2040 and CC1101. For those sub-gigahertz remotes laying around. 

## Acknowledgements
### CC1101-TOOL

All credit goes to **LSatan** for this amazing work at: https://github.com/LSatan/SmartRC-CC1101-Driver-Lib

### RP2040 MQTT

All credit goes to **Nick O’Leary - @knolleary** for this amazing work at: https://github.com/knolleary/pubsubclient

Use of the example to connect to a mqtt topic and parse the commands as if a serial interface where used. This is implemented with the multicore capabilities of the RP2040.

### RP2040 Multicore and FIFO

All credit goes to **Earle F. Philhower, III** for this amazing work at: https://github.com/earlephilhower/arduino-pico

I´ve made an example to parse a pointer addres instead of whole commands to ease of use here (I might create a pull request): https://github.com/cegea/arduino-pico/blob/example/multicore_fifo/libraries/rp2040/examples/MulticoreFIFO/MulticoreFIFO.ino

### Arduino

All the Arduino libraries available.