==============
EXT IRQ driver
==============

The External Interrupt driver allows external pins to be
configured as interrupt lines. Each interrupt line can be
individually masked and can generate an interrupt on rising,
falling or both edges, or on high or low levels. Some of
external pin can also be configured to wake up the device
from sleep modes where all clocks have been disabled.
External pins can also generate an event.

Features
--------
* Initialization and de-initialization
* Enabling and disabling
* Detect external pins interrupt

Applications
------------
* Generate an interrupt on rising, falling or both edges,
  or on high or low levels.

Dependencies
------------
* GPIO hardware

Concurrency
-----------
N/A

Limitations
-----------
N/A

Knows issues and workarounds
----------------------------
N/A

