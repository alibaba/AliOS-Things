#!/bin/bash

# 1、check whether py_engine cmake build success 
echo "1、check whether py_engine cmake build success:${BOARD}"
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
echo "2、export idf env for image making:${BOARD}"
basedir=$(cd $(dirname ${BASH_SOURCE[0]}); pwd )
if [ $BOARD = "nodemcu32s_c3" ]; then
source $basedir/../../hardware/chip/espressif_esp32_c3/esp-idf/export.sh
else
source $basedir/../../hardware/chip/espressif_esp32/esp-idf/export.sh
fi

# 3、make image
echo "3、make image:${BOARD}"
if [ $BOARD = "nodemcu32s_c3" ]; then
esptool.py --chip esp32c3 elf2image out/py_engine_esp32_demo/aos.elf --output aos.bin --flash_freq 40m --flash_mode dio
else
esptool.py --chip esp32 elf2image out/py_engine_esp32_demo/aos.elf --output aos.bin --flash_freq 40m --flash_mode dio
fi
./build_bin.py
python3 makefs.py partition-table.bin
python3 makeimg.py sdkconfig bootloader.bin partition-table.bin aos.bin fs.bin firmware.bin

echo ""
echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
echo "^^^^^^^^^^^^^ makeimg success ... ^^^^^^^^^^^^^"
echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
echo ""
