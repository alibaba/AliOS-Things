power_modes_test application
============================

Demonstrates low power modes of the i.MX6 series devices.


Description
-----------

The power_modes_test application shows how to enter and exit the low power Wait and Stop
modes of the i.MX6 devices.

After the application starts running, it sets up the EPIT2 peripheral to be used as an interrupt
source to wake up the processor from low power modes. Then it immediately enters the Wait mode.
After approximately 5 seconds, the EPIT2 interrupt fires and wakes up the chip. Next, Stop mode
is entered. Again, after about 5 seconds, the EPIT2 interrupt wakes up the chip.


Requirements
------------

No extra hardware is required.


Build options
-------------

None.


Code organization
-----------------

All code for power_modes_test resides in power_modes_test.c in the src directory. It makes use
of the ccm_pll and epit drivers from the SDK.

