### BTLE_Transfer ###

===========================================================================
DESCRIPTION:

This sample shows how to transfer data from an iOS device in CoreBluetooth Peripheral Mode to another in Central Mode, by using a CBCharacteristic on the Peripheral side that changes its value.  The value change is automatically picked up on the Central side.

This sample shows how to handle flow control in this scenario.

It also covers a rudimentary way of connecting two devices using the RSSI value as an approximation of distance between them.

===========================================================================
BUILD REQUIREMENTS:

Xcode 4.5 with iOS 6 SDK

===========================================================================
RUNTIME REQUIREMENTS:

iOS 6

===========================================================================
PACKAGING LIST:

The sample contains the following items:

AppDelegate.[hm] - ancillary code file for application delegate support.

ViewController.[hm] - ancillary code file for application main view controller support.

BTLECentralViewController.[hm] - A view controller which supports the Bluetooth Central function.

BTLEPeripheralViewController.[hm] - A view controller which supports the Bluetooth Peripheral function.

TransferService.h - Transfer service and characteristic UUID declarations.

===========================================================================

Using the sample

1. Run the sample on two devices which support Bluetooth LE and have iOS 6 installed

2. On one device, press the Central button. This will be the Central mode device. The device will begin scanning for a peripheral device which is advertising the Transfer Service.

3. On the other device, press the Peripheral button. This will be the Peripheral mode device.

4. On the Peripheral mode device, press the advertise on/off switch to enable peripheral mode advertising of the data in the text field.

5. Bring the 2 devices into proximity

The Central mode device detects and connects to the Transfer service device, finds the Transfer service characteristics and requests updates to the characteristic. The peripheral mode sends the text in the text field, then sends the EOM string to indicate no further data will be sent. The Central mode device detects the final EOM string and disconnects from the Peripheral mode device. 

The Central mode device will begin scanning again and will repeat the process above until the Central mode device stops searching, or the Peripheral mode device stops advertising.

You can return to the main view and switch the function of each device to transfer data in the reverse direction.

===========================================================================
CHANGES FROM PREVIOUS VERSIONS:

Version 1.0
- First version.

===========================================================================
Copyright (C) 2012 Apple Inc. All rights reserved.
