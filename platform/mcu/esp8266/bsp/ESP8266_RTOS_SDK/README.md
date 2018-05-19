# ESP8266_RTOS_SDK #

----------

ESP8266 SDK based on FreeRTOS.
   
## Note ##

APIs of "ESP8266_RTOS_SDK" are same as "ESP8266_NONOS_SDK"

More details in "Wiki" !

## Requrements ##

You can use both xcc and gcc to compile your project, gcc is recommended.
For gcc, please refer to [esp-open-sdk](https://github.com/pfalcon/esp-open-sdk).

  
## Compile ##

Clone ESP8266_RTOS_SDK, e.g., to ~/ESP8266_RTOS_SDK.

    $git clone https://github.com/espressif/ESP8266_RTOS_SDK.git

Modify gen_misc.sh or gen_misc.bat:
For Linux:

    $export SDK_PATH=~/ESP8266_RTOS_SDK
    $export BIN_PATH=~/ESP8266_BIN

For Windows:

    set SDK_PATH=/c/ESP8266_RTOS_SDK
    set BIN_PATH=/c/ESP8266_BIN

ESP8266_RTOS_SDK/examples/project_template is a project template, you can copy this to anywhere, e.g., to ~/workspace/project_template.

Generate bin: 
For Linux:

    ./gen_misc.sh

For Windows:

    gen_misc.bat
   
Just follow the tips and steps.

## Download ##

eagle.app.v6.flash.bin, downloads to flash 0x00000

eagle.app.v6.irom0text.bin, downloads to flash 0x40000

blank.bin, downloads to flash 0x7E000
