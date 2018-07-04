#!/bin/bash

#add your build targets here, one line for one target, in the following format:
#   "target build args" "target binary path" "target binary name after copy"
declare -a targets=(
    "meshapp@mk3060 meshdebug=1 MESHAUTH=1" "out/meshapp@mk3060/binary/meshapp@mk3060.ota.bin" "meshapp@mk3060-meshauth.bin"
    "yts@linuxhost" "out/yts@linuxhost/binary/yts@linuxhost.elf" "yts@linuxhost.elf"
    "networkapp@stm32l432kc-nucleo" "out/networkapp@stm32l432kc-nucleo/binary/networkapp@stm32l432kc-nucleo.bin" "networkapp@stm32l432kc-nucleo-general.bin"
    "acapp@mk3060" "out/acapp@mk3060/binary/acapp@mk3060.bin" "acapp@mk3060-general.bin"
    "acapp@esp32devkitc" "out/acapp@esp32devkitc/binary/acapp@esp32devkitc.bin" "acapp@esp32-general.bin"
    "acapp@stm32l432kc-nucleo" "out/acapp@stm32l432kc-nucleo/binary/acapp@stm32l432kc-nucleo.bin" "acapp@stm32-general.bin"
    "acapp@starterkit" "out/acapp@starterkit/binary/acapp@starterkit.bin" "acapp@starterkit-general.bin"
    "acapp@developerkit" "out/acapp@developerkit/binary/acapp@developerkit.bin" "acapp@developerkit-general.bin"
    "helloworld@mk3060" "out/helloworld@mk3060/binary/helloworld@mk3060.bin" "helloworld@mk3060-general.bin"
    "helloworld@esp32devkitc" "out/helloworld@esp32devkitc/binary/helloworld@esp32devkitc.bin" "helloworld@esp32-general.bin"
    "helloworld@stm32l432kc-nucleo" "out/helloworld@stm32l432kc-nucleo/binary/helloworld@stm32l432kc-nucleo.bin" "helloworld@stm32-general.bin"
    "helloworld@starterkit" "out/helloworld@starterkit/binary/helloworld@starterkit.bin" "helloworld@starterkit-general.bin"
    "helloworld@developerkit" "out/helloworld@developerkit/binary/helloworld@developerkit.bin" "helloworld@developerkit-general.bin"
)

git status > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "error: not in any git repository"
    exit 1
fi

cd $(git rev-parse --show-toplevel)

if [ -d prebuild ] || [ -f prebuild ]; then
    rm -rf prebuild
fi
mkdir prebuild
mkdir testscripts
cp -f tools/testbed/testscripts/*.py testscripts/

arraylength=${#targets[@]}
if [ $((arraylength%3)) -ne 0 ];then
    echo "error: prebuild parameter error"
fi

aos make clean > /dev/null 2>&1
rm -f build.log
for (( i=1; i<${arraylength}+1; i=i+3 ));do
    target=${targets[$i-1]}
    if [ "$(uname)" != "Linux" ] && [[ ${target} == *linuxhost* ]]; then continue; fi
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
