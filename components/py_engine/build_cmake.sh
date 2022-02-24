#!/bin/bash

set -e

if [ "$(uname)" == "Darwin" ]; then
    os="Darwin"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    os="Linux"
elif ["$(expr substr $(uname -s) 1 10)"=="MINGW32_NT"];then
    os="Win32"
fi
echo "os is $os"

if [[ -f $1 ]]; then
# read arguments from file, and choose those arguments we wanted
filename=$1
while read line; do
    key=$(echo $line | cut -d '=' -f1)
    if [ "$os" == "Darwin" ]; then
        value=$(echo $line | cut -d '=' -f2-)
    elif [ "$os" == "Linux" ]; then
        value=$(echo $line | cut -d '=' --complement -f1)
    else
        value=""
    fi
    case "$key" in
        --toolchain) toolchain=$(echo $value | sed -e 's/^"//' -e 's/"$//');;
        --lib) lib=$(echo $value | sed -e 's/^"//' -e 's/"$//');;
        --target) target=$(echo $value | sed -e 's/^"//' -e 's/"$//');;
        --cflag) cflag=$(echo $value | sed "s/#/ /g");;
        --cxxflag) cxxflag=$(echo $value | sed "s/#/ /g");;
        --asmflag) asmflag=$(echo $value | sed "s/#/ /g");;
        --ldflag) ldflag=$(echo $value | sed "s/#/ /g");;
        --cpu) cpu=$(echo $value | sed -e 's/^"//' -e 's/"$//');;
        --macro_list) macro_list=$(echo $value | sed "s/#/ /g");;
        --global_inc) global_inc=$(echo $value | sed "s/#/ /g");;
        --solution_dir) solution_dir=$(echo $value | sed 's/\"//g');shift;;
        *) echo "discard option $line";;
    esac;
done < "$filename"
else
# choose those arguments we wanted, and discard others
until [ $# -eq 0 ]
do
    key=$(echo $1 | cut -d '=' -f1)
    value=$(echo $1 | cut -d '=' --complement -f1)
    case "$key" in
        --toolchain) toolchain=$value;shift;;
        --lib) lib=$value;shift;;
        --target) target=$value;shift;;
        --cflag) cflag=$(echo $value | sed "s/#/ /g");shift;;
        --cxxflag) cxxflag=$(echo $value | sed "s/#/ /g");shift;;
        --asmflag) asmflag=$(echo $value | sed "s/#/ /g");shift;;
        --ldflag) ldflag=$(echo $value | sed "s/#/ /g");shift;;
        --cpu) cpu=$value;shift;;
        --macro_list) macro_list=$(echo $value | sed "s/#/ /g");shift;;
        --global_inc) global_inc=$(echo $value | sed "s/#/ /g");shift;;
        --solution_dir) solution_dir=$(echo $value | sed 's/\"//g');shift;;
        *) echo "discard option $1";shift;;
    esac;
done
fi

# echo "the script is $0"
# echo "current_dir is $(pwd)"
# echo "solution_dir is $solution_dir"
# echo "toolchain is $toolchain"
# echo "lib is $lib"
# echo "target is $target"
# echo "cflag is $cflag"
# echo "cxxflag is $cxxflag"
# echo "asmflag is $asmflag"
# echo "ldflag is $ldflag"
# echo "cpu is $cpu"
# echo "macro_list is $macro_list"
# echo "global_inc is $global_inc"

# =======================================================
# do our work
comp_dir=$(dirname $0)
echo "comp_dir is $comp_dir"
modules_dir=${comp_dir}/modules

# =======================================================
# Check target board
if [[ $macro_list =~ "HAAS_EDU_K1" ]]; then
    TARGET_BOARD="HaaSEDUk1"
    TARGET_PLATFORM="HAAS"
elif [[ $macro_list =~ "HAAS200" ]]; then
    TARGET_BOARD="HaaS200"
    TARGET_PLATFORM="HAAS"
elif [[ $macro_list =~ "HAAS506" ]]; then
    TARGET_BOARD="HaaS506"
    TARGET_PLATFORM="HAAS"
elif [[ $macro_list =~ "M5STACKCORE2" ]]; then
    TARGET_BOARD="M5STACK_CORE2"
    TARGET_PLATFORM="ESP32"
elif [[ $macro_list =~ "NODEMCU32C3" ]]; then
    TARGET_BOARD="GENERIC_C3"
    TARGET_PLATFORM="ESP32"
else
    TARGET_BOARD="GENERIC"
    TARGET_PLATFORM="ESP32"
fi
echo "TARGET_BOARD is $TARGET_BOARD"
echo "TARGET_PLATFORM is $TARGET_PLATFORM"

# ======= Generate config.cmake file =========
echo "# Automatically generated by call_cmake.sh, DO NOT MODIFY" > ${modules_dir}/config.cmake

# Generate MACRO Defination
macro_list_t=$(echo ${macro_list//\-D/ })
mpy_module_key="MICROPY_PY"
mpy_module_value="1"
IFS=' ' read -r -a array <<< $macro_list_t
for element in "${array[@]}"
do
    if [[ $element =~ $mpy_module_key ]]; then
        echo "$element"
        if [[ $element =~ $mpy_module_value ]]; then
            echo "set(${element%=*} TRUE)" >> ${modules_dir}/config.cmake
        else
            echo "set(${element%=*} FALSE)" >> ${modules_dir}/config.cmake
        fi
    elif [[ $element =~ "SYSINFO_SYSTEM_VERSION" ]]; then
        echo "$element"
        echo "list(APPEND MICROPY_DEF_MODULES_PORT ${element})" >> ${modules_dir}/config.cmake
    fi
done
echo "list(APPEND MICROPY_DEF_MODULES_PORT ${TARGET_BOARD})" >> ${modules_dir}/config.cmake

# =======================================================
if [ $TARGET_PLATFORM = "HAAS" ]; then
    # HaaS platform build
    echo "Start build HAAS target"

    # 0: remove libmicropython lib firstly
    rm -rf ${solution_dir}/aos_sdk/lib/libmicropython.a

    # 1、Generate compile option
    option_array=()
    cflag_t=$(echo ${cflag//})
    IFS=' ' read -r -a array_cflat <<< $cflag_t
    for element in "${array_cflat[@]}"
    do
        if ! [[ $element =~ "-D" ]]; then
            option_array+=($element)
        fi
    done
    option_array=($(awk -v RS=' ' '!a[$1]++' <<< ${option_array[@]}))
    echo "set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS} ${option_array[@]}\")" >> ${modules_dir}/config.cmake
    echo "set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} ${option_array[@]}\")" >> ${modules_dir}/config.cmake
    echo "set(MICROPY_BOARD \"${TARGET_BOARD}\")" >> ${modules_dir}/config.cmake

    # 2、export cross compile toolchain
    export CC=${toolchain}-gcc
    export CXX=${toolchain}-g++

    port_dir=${comp_dir}/adapter/haas
    cd ${port_dir}
    mkdir -p build-${TARGET_BOARD}
    cd build-${TARGET_BOARD}
    cmake ..
    make

    # 3、copy libmicropython.a to lib
    if [ ! -d "${solution_dir}/aos_sdk/lib/" ]; then
        mkdir -p ${solution_dir}/aos_sdk/lib/
    fi
    cp -rf libmicropython.a ${solution_dir}/aos_sdk/lib/

elif [ $TARGET_PLATFORM = "ESP32" ]; then
    # ESP32 platform build
    echo "Start build ESP32 target"

    # 0: remove esp libs firstly
    rm -rf ${solution_dir}/esp_sdk/lib/*

    # 1: prepare IDF env
    if [ ${TARGET_BOARD} = "GENERIC_C3" ]; then
        ${comp_dir}/../../hardware/chip/espressif_esp32_c3/build.sh
        . ${comp_dir}/../../hardware/chip/espressif_esp32_c3/esp-idf/export.sh
    else
        ${comp_dir}/../../hardware/chip/espressif_esp32/build.sh
        . ${comp_dir}/../../hardware/chip/espressif_esp32/esp-idf/export.sh
        export ADF_PATH=${comp_dir}/../../hardware/chip/espressif_esp32/esp-adf
    fi

    # 2：prepare fs for esp target
    port_dir=${comp_dir}/adapter/esp32
    rm -rf ${solution_dir}/fs
    cp -rf ${port_dir}/fs  ${solution_dir}/
    if [ ${TARGET_BOARD} = "M5STACK_CORE2" ]; then
        cp -rf ${port_dir}/m5stackcore2/*  ${solution_dir}/fs/
    fi

    # 3: prepare fs for esp target
    cd ${port_dir}
    make BOARD=${TARGET_BOARD}

    # 4: copy generated static libs to solutions
    cd build-${TARGET_BOARD}

    mkdir -p ${solution_dir}/esp_sdk/lib
    mkdir -p ${solution_dir}/esp_sdk/include
    find esp-idf \( -name "*.a" -o  -name "*.ld" \) -exec cp '{}' ${solution_dir}/esp_sdk/lib/ \;
    cp ./esp-idf/mbedtls/mbedtls/library/libmbedtls.a ${solution_dir}/esp_sdk/lib/
    cp -f sdkconfig ${solution_dir}
    cp -f bootloader/bootloader.bin ${solution_dir}
    cp -f partition_table/partition-table.bin ${solution_dir}
    cp -f config/sdkconfig.h ${solution_dir}/esp_sdk/include/
else
    echo "No target build !!!!"
fi

# =======================================================
# result
echo "run external script success"
exit 0