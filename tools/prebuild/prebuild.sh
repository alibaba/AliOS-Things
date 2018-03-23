#!/bin/bash

#add your build targets here, one line for one target, in the following format:
#   "target build args" "target binary path" "target binary name after copy"
declare -a targets=(
    "alinkapp@mk3060 meshdebug=1" "out/alinkapp@mk3060/binary/alinkapp@mk3060.ota.bin" "alinkapp@mk3060-general.bin"
    "meshapp@mk3060 meshdebug=1 MESHAUTH=1" "out/meshapp@mk3060/binary/meshapp@mk3060.ota.bin" "meshapp@mk3060-meshauth.bin"
    "alinkapp@esp32devkitc meshdebug=1" "out/alinkapp@esp32devkitc/binary/alinkapp@esp32devkitc.bin" "alinkapp@esp32-general.bin"
    "yts@linuxhost" "out/yts@linuxhost/binary/yts@linuxhost.elf" "yts@linuxhost.elf"
    "networkapp@stm32l432kc-nucleo" "out/networkapp@stm32l432kc-nucleo/binary/networkapp@stm32l432kc-nucleo.bin" "networkapp@stm32l432kc-nucleo-general.bin"
)

git status > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: not in any git repository"
    exit 1
fi

cd $(git rev-parse --show-toplevel)

if [ ! -f build/autobuild.sh ]; then
    echo "error: can not find autobuild.sh script"
fi
bash build/autobuild.sh
if [ $? -ne 0 ];then
    echo "error: run autobuild.sh failed"
    exit 1
fi
echo "run autobuild.sh succeed"

if [ -d prebuild ] || [ -f prebuild ]; then
    rm -rf prebuild
fi
mkdir prebuild

arraylength=${#targets[@]}
if [ $((arraylength%3)) -ne 0 ];then
    echo "error: prebuild parameter error"
fi

aos make clean > /dev/null 2>&1
rm -f build.log
for (( i=1; i<${arraylength}+1; i=i+3 ));do
    aos make ${targets[$i-1]} > build.log 2>&1
    if [ $? -ne 0 ]; then
        echo "error: build '${targets[$i-1]}' failed, log:"
        cat build.log
        rm -f build.log
        exit 1
    fi
    echo "build '${targets[$i-1]}' succeed"
    rm -f build.log

    cp ${targets[$i]} prebuild/${targets[$i+1]}
    if [ $? -ne 0 ]; then
        echo "error: cp '${targets[$i]}' failed"
        exit 1
    fi
    aos make clean > /dev/null 2>&1
done
