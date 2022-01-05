#!/bin/bash

echo "start to import esp-idf env"

# get current path
basedir=$(cd $(dirname ${BASH_SOURCE[0]}); pwd )
source $basedir/../../hardware/chip/espressif_esp32/esp-idf/export.sh

echo "start to run build_cmake ......"

if [ "$(uname)" == "Darwin" ]; then
    os="Darwin"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    os="Linux"
elif ["$(expr substr $(uname -s) 1 10)"=="MINGW32_NT"];then
    os="Win32"
fi

echo "OS is $os ..........."

esptool.py --chip esp32 elf2image out/py_engine_esp32_demo/aos.elf --output aos.bin --flash_freq 40m --flash_mode dio
./build_bin.py
python3 makefs.py partition-table.bin
python3 makeimg.py sdkconfig bootloader.bin partition-table.bin aos.bin fs.bin firmware.bin

echo "*****************************"
echo "end to run makeimg !!!!      "
echo "*****************************"
echo ""
