#!/bin/bash

# 1、check whether py_engine cmake build success 
if [ -d "esp_sdk/lib" ]; then
    if [ ! -f "esp_sdk/lib/libmain.a" ]; then
        echo ""
        echo "***********************************************"
        echo "******** py_engine Cmake Build Failed *********"
        echo "***********************************************"
        echo ""
        exit -1
    fi
fi

# 2、export idf env for image making
basedir=$(cd $(dirname ${BASH_SOURCE[0]}); pwd )
source $basedir/../../hardware/chip/espressif_esp32/esp-idf/export.sh

# 3、make image
esptool.py --chip esp32 elf2image out/py_engine_esp32_demo/aos.elf --output aos.bin --flash_freq 40m --flash_mode dio
./build_bin.py
python3 makefs.py partition-table.bin
python3 makeimg.py sdkconfig bootloader.bin partition-table.bin aos.bin fs.bin firmware.bin

echo ""
echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
echo "^^^^^^^^^^^^^ makeimg success ... ^^^^^^^^^^^^^"
echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
echo ""
