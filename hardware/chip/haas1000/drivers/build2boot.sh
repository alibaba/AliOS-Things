#!/bin/bash

export CUR_PATH=$(pwd)
export PATH=$CUR_PATH/../../../../build/compiler/gcc-arm-none-eabi/Linux64/bin/:$PATH

make T=ota_boot2a clean --print-directory
make T=ota_boot2a CPU=m4 SECURE_BOOT=1 REMAP_SUPPORT=1 BOOTINFO_BIN=1 HAAS_OTA_ENABLED=1 EXTERNAL_WATCHDOG=1 SDK=1 HW_MODULE=1 LARGE_RAM=1 all lst --print-directory
cp $CUR_PATH/out/ota_boot2a/ota_boot2a.bin $CUR_PATH/../prebuild/ota_boot2a.bin