usb_hid_mouse application
=========================

Example of a USB device HID class mouse.


Description
-----------

This application runs a USB device stack and enumerates as a HID class mouse. It is based on
the HID class example application that comes with the bare-metal USB stack, extended to send
reports without any required user interaction.

After enumeration is complete, the firmware repeatedly sends reports to the host so that the
cursor moves in a circle.


Requirements
------------

USB connected to a host machine such as a Windows, Mac, or Linux PC.


Build options
-------------

None.


Code organization
-----------------

mouse_button.c - The main() function, initialization, and class handlers.

usb_descriptor.c - USB descriptors and endpoint definitions and related functions.


