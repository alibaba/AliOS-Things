The Source code for the Timberwolf device driver is partitioned into 4 folders.

\api_lib\
There are 5 files in this folder
The VprocTwolf_access.c/h -  
is the user's space OS independent API to read/write specific register of the device to 
reset the device into one of the 5 supported reset modes and to boot load a firmware/configuration record into the device.
The API must be used in conjunction with a low level device driver such as microsemi_spis_tw.c/h, VprocGal_HAL.c/h

vproc_common.c/h - 
includes functions and variable declarations that are common to all Microsemi Voice processing devices.
These functions include the Vproc_msDelay(), VprocWait(). The Variables declarations include 
the device status codes, and device reset modes enums. As well as multiple macros to enable debug mode.

the specific user_space hardware abstraction layer code for the ZL38040/05x/06x/08x Timberwolf devices.

\firmware\
This folder contains the firmware image , configuration record files and header file. 

NOTE: The firmware of the development board and its corresponding configuration have been encapsulated into a static library.

\example_apps\
This folder contains example host applications for the zl38040/050/060/080 Microsemi devices.

zl38063.c/h -
a series of operations for zl38063, such as initialize, adjust the volume, and so on.


NOTE: You can get the latest firmware and related technical support by registering SDS account.
	  http://sds.microsemi.com/software.php.

	  ZLS38063 GUI software(MiTuner Lite GUI Software), ZLS38063 and ZLS38508LITE
	  http://sds.microsemi.com/software.php?view_type=listrev&id=103386.

	  firmware of ZL38063
	  http://sds.microsemi.com/software.php?view_type=listrev&id=104598.