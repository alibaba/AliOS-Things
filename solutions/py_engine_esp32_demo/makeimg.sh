#!/bin/bash

set -e

# 1、Get board type from .config file
shellPath=$(cd $(dirname ${BASH_SOURCE[0]}); pwd )
configPath=${shellPath}/../../
configPath="`cd "${configPath}";pwd`"
configFile=${configPath}/.config

if [ ! -f "${configFile}" ]; then
    echo ""
    echo "****************************************"
    echo "******* .config file not exist *******"
    echo "****************************************"
    echo ""
    exit -1
fi

# Check target board
configInfo=$(<${configFile})
if [[ $configInfo =~ "m5stackcore2" ]]; then
    TARGET_PLATFORM="ESP32"
elif [[ $configInfo =~ "esp_c3_32s_kit" ]]; then
    TARGET_PLATFORM="ESP32_C3"
elif [[ $configInfo =~ "esp_s3_12k_kit" ]]; then
    TARGET_PLATFORM="ESP32_S3"
else
    TARGET_PLATFORM="ESP32"
fi

echo ""
echo "******** TARGET_PLATFORM is ${TARGET_PLATFORM} ********"
echo ""

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
if [ $TARGET_PLATFORM = "ESP32" ]; then
    esptool.py --chip esp32 elf2image out/py_engine_esp32_demo/aos.elf --output aos.bin --flash_freq 40m --flash_mode dio
elif [ $TARGET_PLATFORM = "ESP32_C3" ]; then
    esptool.py --chip esp32c3 elf2image out/py_engine_esp32_demo/aos.elf --output aos.bin --flash_freq 40m --flash_mode dio
elif [ $TARGET_PLATFORM = "ESP32_S3" ]; then
    esptool.py --chip esp32s3 elf2image out/py_engine_esp32_demo/aos.elf --output aos.bin --flash_freq 40m --flash_mode dio
fi

./build_bin.py
python3 makefs.py partition-table.bin
python3 makeimg.py sdkconfig bootloader.bin partition-table.bin aos.bin fs.bin firmware.bin

echo ""
echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
echo "^^^^^^ make firmware for target (${TARGET_PLATFORM}) success ^^^^^^"
echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
echo ""
