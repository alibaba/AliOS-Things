obds application
================

On-Board Diagnostic Suite used for comprehensive board hardware testing.


Description
-----------

The On-Board Diagnostic Suite (obds) application is a collection of tests used to validate the 
connectivity of all the peripherals found on the various Freescale reference boards. This test suite
is used by the board manufacturers to verify the board assembly before they are placed in inventory.    

There are two basic modes of operation. A menu is provided at startup. Choosing the first item in the
menu, 00 ALL Tests, will cause the program to iterate through each of the tests listed in the menu.
Alternatively, the user can select each test interactively by selecting its corresponding menu item.
In this mode, the program will revert back to the menu selection after the execution of each test so
the user can re-run the same test or run other tests in any order.

Requirements
------------

Many OBDS tests require extra hardware to be attached to the board being used. If extra hardware,
loop-back cables, displays, etc. are required, the test will display the requirements in the terminal window.
In some instances, users will be given a choice of supported displays to choose from. Typical instructions
might include setting a jumper to a certain position, or insert an SD card into a specified port. 

Build options
-------------

None.


Code organization
-----------------

obds.c in the src directory implements the main() function and the menu interface for all tests.

The majority of the test implementations can be found in the src/tests directory; however some tests
are represented either directly or indirectly by the sdk/drivers/[ip_block]/test files that constitute
the sdk_unit_tests.

The application is built specifically for each CPU/Board/Board_Rev, and tests will vary from board to board
depending on the peripherals supported by a given board.

The tests included are specified by the select_test() function in obds.c and the following definitions of
CPU_Tests[], MainBoard_Tests[], and Display_Test[];

Known defects
-----------------

The following tests are included in the test suite even though they are currently failing at some level.
* RGMII AR8031 G-Ethernet Test
* SPI NOR FLASH Test
* USB OTG Device Enumeration Test
* USB Host1 Device Enumeration Test
* IPU Display Tests
* Light Sensor ISL29023 I2C Device ID Test
* FlexCan1 <-> FlexCan2 Loopback Test


