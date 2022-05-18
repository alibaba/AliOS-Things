#!/bin/bash
for dir in esp32 esp32s2 esp32c3 esp32s3; do
    if [ $dir = esp32 ]; then
        TOOLCHAIN="xtensa-esp32-elf"
    elif [ $dir = esp32s2 ]; then
        TOOLCHAIN="xtensa-esp32s2-elf"
    elif [ $dir = esp32c3 ]; then
        TOOLCHAIN="riscv32-esp-elf"
    elif [ $dir = esp32s3 ]; then
        TOOLCHAIN="xtensa-esp32s3-elf"
    else
        echo "$dir does not exist"
    fi
    if [ -d "$dir" ]; then
        cd $dir

        if [ $dir = esp32 ] || [ $dir = esp32s2 ]; then
            git status librtc.a | grep "modified" >/dev/null 2>&1
            if [ $? -eq 0 ]; then
                echo $dir/librtc.a fixed
                $TOOLCHAIN-objcopy --redefine-sym ets_printf=rtc_printf librtc.a
            fi
        fi

        git status libphy.a | grep "modified" >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo $dir/libphy.a fixed
            $TOOLCHAIN-objcopy --redefine-sym ets_printf=phy_printf libphy.a
        fi

        cd ..
    else
        echo "$dir does not exist"
    fi
done;
