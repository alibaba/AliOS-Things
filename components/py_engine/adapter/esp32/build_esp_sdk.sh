#!/usr/bin/env bash

#set -x

CUR_FILE_PATH="$(dirname $0)"
PROJECT_PATH="${CUR_FILE_PATH}/../../../../solutions/py_engine_esp32_demo"

if grep "DBOARD_M5STACKCORE2=1" $1 ; then
    TARGET_BOARD="M5STACK_CORE2"
else
    TARGET_BOARD="GENERIC"
fi

${CUR_FILE_PATH}/../../../../hardware/chip/espressif_esp32/build.sh
. ${CUR_FILE_PATH}/../../../../hardware/chip/espressif_esp32/esp-idf/export.sh
export ADF_PATH="${CUR_FILE_PATH}/../../../../hardware/chip/espressif_esp32/esp-adf"


rm -rf ${PROJECT_PATH}/fs
cp -rf  ${CUR_FILE_PATH}/fs  ${PROJECT_PATH}/
if [ ${TARGET_BOARD} = "M5STACK_CORE2" ]; then
    cp -rf  ${CUR_FILE_PATH}/m5stackcore2/*  ${PROJECT_PATH}/fs/
fi


cd ${CUR_FILE_PATH}
make BOARD=${TARGET_BOARD}

cd build-${TARGET_BOARD}

find esp-idf \( -name "*.a" -o  -name "*.ld" \) -exec cp '{}' ${PROJECT_PATH}/esp_sdk/lib/ \;
cp ./esp-idf/mbedtls/mbedtls/library/libmbedtls.a ${PROJECT_PATH}/esp_sdk/lib/
cp -f sdkconfig ${PROJECT_PATH}
cp -f bootloader/bootloader.bin ${PROJECT_PATH}
cp -f partition_table/partition-table.bin ${PROJECT_PATH}
cp -f config/sdkconfig.h ${PROJECT_PATH}/esp_sdk/include/

if [ ${TARGET_BOARD} = "M5STACK_CORE2" ]; then
    cat ${CUR_FILE_PATH}/boards/M5STACK_CORE2/sdkconfig.frag.h >> ${PROJECT_PATH}/esp_sdk/include/sdkconfig.h
fi

exit 0
