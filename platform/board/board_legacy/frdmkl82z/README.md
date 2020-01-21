## Overview

The board FRDM-KL82Z is produced by NXP semiconductor, it supports a power supply voltage range of 1.71 to 3.6 V. It features a KL82Z, a device boasting up to 128 KB flash and 96 KB SRAM, and numerous analog and digital peripherals. In addition, the FRDM-KL82Z offers a QSPI interface to expand program memory and USB 2.0 Full Speed without requiring an external crystal.

## Feature of Board
The FRDM-KL82Z hardware is an NXP Freedom development board assembled with the following features:
- Board power select with 3.3 V or 1.8 V MCU operation.
- MKL82Z128VLK7 MCU (96 MHz, 128 KB flash, 96 KB SRAM, 80 LQFP package)
- Accelerometer and magnetometer, FXOS8700CQ.
- Onboard debug circuit: MK20DX128VFM5 OpenSDA with virtual serial port.
- I/O headers for easy access to MCU I/O pins.
- Flexible power supply options: USB, coin cell battery, external source.
- Two TSI(Touch Sensing Interface) touch pads.
- Reset push button.
- Two mechanical push buttons, NMI and LLWU buttons.
- RGB LED.
	
## Steps to download and run the yts case.
1. Run the command to build the image:
   > "aos make yts@fremkl82z test=certificate"
2. Connect a USB cable between the host PC and the OpenSDA USB port on the target board.
3. Open a serial terminal with the following settings:
   - 115200 baud rate
   - 8 data bits
   - No parity
   - One stop bit
   - No flow control
4. Download the program to the target board.
5. Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.
6. If the test case run successfully, you can see all 26 test cases run pass.

## Update log
- Initial update (7-20-2018):
  > Porting the alios to frdmkl82z board, the yts case can all run pass on this board.


