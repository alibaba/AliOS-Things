## Overview

The Freedom KL26Z is an ultra-low-cost development platform for Kinetis® L series KL16 and KL26 MCUs (up to 128 KB Flash and up to 64-pin packages) built on Arm® Cortex®-M0+ processor.

## Feature of Board

- MKL26Z128VLH4 MCU – 48 MHz, 128 KB flash, 16 KB SRAM, USB OTG (FS), 64 LQFP.
- Capacitive touch "slider," FXOS8700CQ magnetometer and accelerometer, tri-color LED.
- Easy access to MCU I/O.
- Sophisticated OpenSDA debug interface.
- Mass storage device flash programming interface (default) – no tool installation required to evaluate demo apps.
- P&E Multilink interface provides run-control debugging and compatibility with IDE tools.
- Open-source data logging application provides an example for customer, partner and enthusiast development on the OpenSDA circuit.
	
## Steps to download and run the yts case.

1. Due to the limitation of RAM size(16Kb only), please do the changes the files like below:
   - In file test/testcase/certificate_test/aos_test.c:
     > Change TEST_CONFIG_YLOOP_EVENT_COUNT to 50
     > Change TEST_CONFIG_YLOOP_LOOP_COUNT  to 2
   - In file kernel/cli/include/cli_conf.h:
     > Change CLI_CONFIG_STACK_SIZE to 1024
   - In file kernel/cli/include/cli_conf.h:
     > Change CLI_CONFIG_OUTBUF_SIZE to 256
2. Run the command to build the image:
   > "aos make yts@fremkl26z test=certificate"
3. Connect a USB cable between the host PC and the OpenSDA USB port on the target board.
4. Open a serial terminal with the following settings:
   - 115200 baud rate
   - 8 data bits
   - No parity
   - One stop bit
   - No flow control
5. Download the program to the target board.
6. Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.
7. If the test case run successfully, you can see yts test case run pass.

## Update log
- Initial update (7-20-2018):
  > Porting the alios to frdmkl26z board, the yts case can all run pass on this board.
