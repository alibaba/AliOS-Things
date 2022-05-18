#!/bin/bash

set -e

driver_libs=("cht8305" "si7006" "sh1106" "haaseduk1" "mpu6050" "qmi8610" "ap3216c" "qmc5883" "qmc6310" "qmp6988" "spl06" "display_driver")

echo "Start to copy HaaS driver libs to prebuild folder ......"

targetBoard=$1
echo "targetBoard=${targetBoard}"

shellPath=$(cd $(dirname ${BASH_SOURCE[0]}); pwd )

hardwarePath=${shellPath}/../../hardware
hardwarePath="`cd "${hardwarePath}";pwd`"

driverSrcPath=${shellPath}/../../haas_lib_bundles/python/libraries
driverSrcPath="`cd "${driverSrcPath}";pwd`"


if [ ${targetBoard} = "HaaS100" ]; then
    libs_dst=${hardwarePath}/chip/haas1000/prebuild/data/data/lib
    fs_root=${hardwarePath}/chip/haas1000/prebuild/data/
elif [ ${targetBoard} = "HaaS200" ]; then
    libs_dst=${hardwarePath}/chip/rtl872xd/prebuild/data/data/lib
    fs_root=${hardwarePath}/chip/rtl872xd/prebuild/data/
elif [ ${targetBoard} = "HaaSEDUk1" ]; then
    libs_dst=${hardwarePath}/chip/haas1000/prebuild/data/data/lib
    fs_root=${hardwarePath}/chip/haas1000/prebuild/data/
else
    libs_dst=${hardwarePath}/chip/haas1000/prebuild/data/data/lib
    fs_root=${hardwarePath}/chip/haas1000/prebuild/data/

fi

# create lib folder if not exist
if [ ! -d "${libs_dst}" ]; then
    mkdir -p ${libs_dst}
else
    rm -rf ${libs_dst}/*
fi

for lib in ${driver_libs[@]}
do
    temp_file=${driverSrcPath}/${lib}/$lib.py

    if [ ! -f "$temp_file" ]; then
        echo "lib copy error !!!!"
        echo "${temp_file} NOT EXIST"
    else
        cp -rf ${temp_file} ${libs_dst}
    fi

    temp_file=${driverSrcPath}/${lib}/board.json
    if [ -f "$temp_file" ]; then
        cp -rf ${temp_file} ${libs_dst}
    fi

done

# cp boot.py and appOta.py for haas devices

cp -rf ${shellPath}/adapter/haas/fs/boot.py ${fs_root}
cp -rf ${shellPath}/adapter/haas/fs/lib ${fs_root}

exit 0

