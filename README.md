# ln2block

ln2block is the combination of hardware and software to send block messages in a Fremo meeting between two block enabled stations. There are more implementations for this, which can be found at [fremo-block](http://fremo-block.sourceforge.net/) The ln2block variant aims to directly connect Loconet switchpanels to block. First implementation is done with https://smf-modelle.de/ Dr-S2 panels in the Loconet variant. But other implementations such as arduino based or felderblock are possible.

## hardware

The hardware is a adaption of the boards shown [here](http://fremo-block.sourceforge.net/Loconet2Block/index.html) with the following adaptations:
- Schlüesselinterface integrated
- Arduino Nano based to ease programming and debugging
- OLED status display

v5 is a variant which has connections on both ends of the pcb.
![v5 prototype](https://raw.githubusercontent.com/nh-finescale/ln2block/main/documentation/img/ln2bv5.jpeg)

v6 is adapted to fit the Strapubox SP2000 (again).

## Getting parts:

Reichelt Shopping Cart for V5 parts: (https://www.reichelt.de/my/1871884). For logging purposes a SSD1306 OLED Display is used. Any I2C variant should work.

## Manuals:

See [the wiki](https://github.com/nh-finescale/ln2block/wiki)

## Software

Software is fully functional for:
* Tf71
* Relaisblock
* Felderblock

### Programming the Arduino

To program the Arduino please use the Arduino IDE.

#### Prerequisites:
* Wire Library (included within IDE): needed for the U8g2 Library
* U8g2 Library: to write text and/or graphic on the OLED display
* LocoNet Library: to send/receive Loconet messages

## Configuration

The ln2block software can be configured through LNCV programming. The values in scope are described at [fremo-block](http://fremo-block.sourceforge.net/Loconet2Block/config.html).
A spreadsheet to calculate the appropriate values for each LNCV is found at https://github.com/nh-finescale/ln2block/tree/main/documentation

This sheet also contains a HEX explanation to translate HEX values shown on the OLED display back into which LNCV are on and off.

## Module connectivity

todo
