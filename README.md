# ln2block

ln2block is the combination of hardware and software to send block messages in a Fremo meeting between two block enabled station. There are more implementations for this, which can be found at [fremo-block](http://fremo-block.sourceforge.net/) The ln2block variant aims to directly connect Loconet switchpanels to block. First implemenation is done with https://smf-modelle.de/ Dr-S2 panels in the Loconet variant. But other implementations such as arduino based or felderblock (still in development here) are possible.

#hardware

The hardware is a adaption of the boards show [here](http://fremo-block.sourceforge.net/Loconet2Block/index.html) with the following adaptations:
- Schlüesselinterface integrated
- Arduino Nano based to ease programming and debugging
- OLED status display

v5 is a variant which has connections on both ends of the pcb
v6 is adapated to fit the Strapubox SP2000 (again)

## Getting parts:

todo

## Manuals:

todo

## Software

old (deprecated) software is found at: [https://github.com/nh-finescale/ln2block/tree/main/src/v0.5%20(deprecated)] it is not advised to use
Manual for programming the Arduino: [Arduino programming](https://github.com/Kleinbahner/BUESte/blob/main/doc/Programmierung%20Arduino%20f%C3%BCr%20Bahn%C3%BCbergangssteuerung.pdf)

## Configuration

The ln2block software can be configured through LNCV programming. The values in scope are described at [fremo-block](http://fremo-block.sourceforge.net/Loconet2Block/config.html).
A spreadsheet to calculate the appropriate values for each LNCV is found at https://github.com/nh-finescale/ln2block/tree/main/documentation

## Module connectivity

todo
