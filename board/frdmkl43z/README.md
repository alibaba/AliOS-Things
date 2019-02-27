## Overview

The FRDM-KL43Z is an ultra-low-cost development platform for Kinetis® L families KL43, KL33, KL27, KL17 and KL13 MCUs built on Arm® Cortex®-M0+ processor running at 48 MHz.

The FRDM-KL43Z is supported by a range of NXP® and third-party development software.

## Feature of Board

- MKL43Z256VLH4MCU – 48MHz, 256KB Flash, 32KBSRAM, 16K ROM based bootloader, segment LCD, USB device (FS), 64 LQFP
- Capacitive touch slider, MMA8451Q accelerometer, MAG3110 magnetometer.
- Flexible power supply options – USB, coin-cell-battery, external source
- Easy access to MCU I/O
- Battery-ready, power-measurement access points
- Form factor compatible with Arduino™ R3 pin layout
- OpenSDA debug interface
- Mass storage device flash programming interface (default) –no tool installation required to evaluate demo apps
- P&E Debug interface provides run-control debugging and compatibility with IDE tools
- CMSIS-DAP interface: new Arm® standard for embedded debug interface Refer to the FRDM-KL43Z User’s Manual and OpenSDA User’s Guide for more information
- Software support and application development are offered through the [Kinetis® software development kit (SDK)](https://www.nxp.com/support/developer-resources/nxp-designs/software-development-kit-for-kinetis-mcus:KINETIS-SDK), an extensive suite of robust peripheral drivers, stacks and middleware (replacing Sample Code Packages/examples). Get expert advice and support by joining the [Kinetis Software Development Kit Community forum](https://community.nxp.com/community/mcuxpresso/kinetis-software-development-kit).

## Steps to download and run the yts case

1. Due to the limitation of RAM size(32k only) and over head of test case, below change was needed:
   - In the file of test/testcase/certificate_test/aos_test.c：
     > Change the TEST_CONFIG_YLOOP_EVENT_COUNT" from 1000 to 10.
2. Run the command to build the image:
   > "aos make yts@fremkl43z test=certificate"
3. Connect a USB cable between the host PC and the OpenSDA USB port on the target board.
4. Open a serial terminal with the following settings:
   - 115200 baud rate
   - 8 data bits
   - No parity
   - One stop bit
   - No flow control
5. Download the program to the target board.
6. Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.
7. If the test case run successfully, you can see all the test cases run pass.

## Update log
- Initial update (7-20-2018):
  > Porting the alios to frdmkl43z board, the yts case can run pass on this board.
