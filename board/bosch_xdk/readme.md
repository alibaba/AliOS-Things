# BOSCH XDK AliOS Things User Guide #

# #

# 1.bootloader flash#
use jlink burn bootloader to flash address 0x0

# 2.application flash#
## none bootloader ##
use jlink burn application bin file to flash address 0x0. 
Debug with XDK through extension port uart0. 

## with bootloader ##
Debug with XDK through extension port uart0 and USB.
### flash with jlink ###
use jlink burn application bin file to flash address 0x10000.
### flash through uart0 ###
-  startup with bootloader
-  connect your PC to extension port uart0 and enter 'u'.
-  send application file through XMODEM-CRC protocol.

reder to XDK user guide link below in chapter 3.6.2
[https://xdk.bosch-connectivity.com/documents/37728/55858/XDK+User+Guide/aad12114-c2c4-4ef1-a8c0-20a5c1e40ff3](https://xdk.bosch-connectivity.com/documents/37728/55858/XDK+User+Guide/aad12114-c2c4-4ef1-a8c0-20a5c1e40ff3)
# 3.build #
    aos make helloworld@bosch_xdk
    aos make mqttapp@bosch_xdk
    aos make linkkitapp@bosch_xdk
    aos make uDataapp@bosch_xdk