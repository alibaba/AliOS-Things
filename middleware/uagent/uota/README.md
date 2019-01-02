This is an uota for the developer to download FW from the server:
app: uota example
inc: include files
hal: platform/OS hal interface
src: uOTA source, service-> transport-> download-> verify

## Linux build
cd uota;
make;

## RTOS build
cd ROOT DIR;
aos make otaapp;


## make sure network is ready to get extern IP address before running
## dhcp for Linux
## netmgr connect ssid pswd for RTOS

run
./OTA_APP pk dn ds
