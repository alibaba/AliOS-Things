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

        git status libsmartconfig.a | grep "modified" >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo $dir/libsmartconfig.a fixed
            $TOOLCHAIN-objcopy --redefine-sym printf=sc_printf libsmartconfig.a
        fi

        git status libpp.a | grep "modified" >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo $dir/libpp.a fixed
            $TOOLCHAIN-objcopy --redefine-sym printf=pp_printf libpp.a
        fi

        git status libnet80211.a | grep "modified" >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo $dir/libnet80211.a fixed
            $TOOLCHAIN-objcopy --redefine-sym printf=net80211_printf libnet80211.a
        fi

        git status libmesh.a | grep "modified" >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo $dir/libmesh.a fixed
            $TOOLCHAIN-objcopy --redefine-sym printf=mesh_printf libmesh.a
            $TOOLCHAIN-objcopy --redefine-sym ets_printf=mesh_printf libmesh.a
        fi

        git status libcore.a | grep "modified" >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo $dir/libcore.a fixed
            $TOOLCHAIN-objcopy --redefine-sym printf=core_printf libcore.a
            $TOOLCHAIN-objcopy --redefine-sym ets_printf=core_printf libcore.a
        fi

        git status libcoexist.a | grep "modified" >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo $dir/libcoexist.a fixed
            $TOOLCHAIN-objcopy --redefine-sym ets_printf=coexist_printf libcoexist.a
            $TOOLCHAIN-objcopy --redefine-sym printf=coexist_printf libcoexist.a
        fi

        git status libespnow.a | grep "modified" >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo $dir/libespnow.a fixed
            $TOOLCHAIN-objcopy --redefine-sym ets_printf=espnow_printf libespnow.a
            $TOOLCHAIN-objcopy --redefine-sym printf=espnow_printf libespnow.a
        fi

        git status libwapi.a | grep "modified" >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo $dir/libwapi.a fixed
            $TOOLCHAIN-objcopy --redefine-sym ets_printf=wapi_printf libwapi.a
            $TOOLCHAIN-objcopy --redefine-sym printf=wapi_printf libwapi.a
        fi

        cd ..
    else
        echo "$dir does not exist"
    fi
done;
