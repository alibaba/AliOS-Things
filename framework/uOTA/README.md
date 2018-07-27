This is an uOTA  for the developer to download FW from the server:
app: uOTA example
inc: include files
hal: platform/OS hal interface
src: uOTA source, service-> transport-> download-> verify

## Linux build
cd uOTA;
make;

## RTOS build
cd uOTA;
aos make uOTA_app;


## make sure network is ready to get extern IP address before running
## dhcp for Linux
## netmgr connect ssid pswd for RTOS

run
./OTA_APP
