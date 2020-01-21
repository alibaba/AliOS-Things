## Overview

The FRDM-KL28Z supports power supply voltage range from 1.71V to 3.6V.It features a KL28Z, a device boasting up to 512KB Flash and 128KB SRAM, and numerous analog and digital peripherals. The onboard interfaces include an RGB LED, a 6-axis digital sensor, a 3-axis digital angular rate gyroscope, an ambient light sensor, and a capacitive touch slider.

## Feature of Board

- MKL28Z512VLL7 MCU (96 MHz, 512 KB Flash, 128 KB RAM, 32 KB ROM,100 LQFP package).
- 3-Axis Digital Angular Rate Gyroscope, FXAS21002CQ ( U2 ).
- Accelerometer and magnetometer, FXOS8700CQ ( U10 ).
- OpenSDA: Onboard serial and debug adapter(U7).
- I/O headers for easy access to MCU I/O pins.
- Ambient light sensor ( Q1 )
- Flexible power supply options :USB, coin cell battery, external source
- Capacitive touch slider
- Reset pushbutton ( SW1 )
- NMI and LLWU buttons ( SW2 , SW3 )
- RGB LED ( D2 )
	
## Steps to download and run the yts case.

1. Run the command to build the image:
   > "aos make yts@fremkl43z test=certificate"
2. Connect a USB cable between the host PC and the OpenSDA USB port on the target board.
3. Open a serial terminal with the following settings:
   - 115200 baud rate
   - 8 data bits
   - No parity
   - One stop bit
   - No flow control
4. Download the program to the target board.
5. Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.
6. If the test case run successfully, you can see yts test case run pass.

## Update log
- Initial update (7-20-2018):
  > Porting the alios to frdmkl28z board, the yts case can all run pass on this board.


