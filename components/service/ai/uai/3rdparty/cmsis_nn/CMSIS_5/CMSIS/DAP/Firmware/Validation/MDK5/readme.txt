CMSIS-DAP debug unit validation
-------------------------------

The following debug functionality is tested:
 - execution breakpoint with hit count
 - breakpoint on read
 - breakpoint on write
 - memory read
 - memory write
 - register read
 - register write
 - single stepping
 - run/stop

The test is self-contained and can be executed on the hardware target.

Test results get printed into a test log file.

To configure the test for specific hardware target:
1) open the uVision project and select device mounted on hardware target
   (automatically selects flash algorithm for download)
2) select CMSIS-DAP debugger (if not already selected)
3) build the project

To run the test on the hardware target:
1) connect hardware target to CMSIS-DAP debug unit via JTAG/SWD
2) connect CMSIS-DAP debug unit to be tested to PC via USB
3) open the uVision project and start debug session

To run the test on the target in batch mode:
   execute test.bat
