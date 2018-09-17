#!/bin/bash

#add your build targets here, one line for one target, in the following format:
#   "target build args" "target binary path" "target binary name after copy"
declare -a targets=(
    "linkkitapp@mk3060 linkkitdebug=1" "out/linkkitapp@mk3060/binary/linkkitapp@mk3060.bin" "linkkitapp@mk3060-general.bin"
    "alinkapp@mk3060 meshdebug=1" "out/alinkapp@mk3060/binary/alinkapp@mk3060.bin" "alinkapp@mk3060-general.bin"
    "meshapp@mk3060 meshdebug=1 MESHAUTH=1" "out/meshapp@mk3060/binary/meshapp@mk3060.bin" "meshapp@mk3060-meshauth.bin"
    "alinkapp@esp32devkitc meshdebug=1" "out/alinkapp@esp32devkitc/binary/alinkapp@esp32devkitc.bin" "alinkapp@esp32-general.bin"
    "yts@linuxhost" "out/yts@linuxhost/binary/yts@linuxhost.elf" "yts@linuxhost.elf"
    "networkapp@stm32l432kc-nucleo" "out/networkapp@stm32l432kc-nucleo/binary/networkapp@stm32l432kc-nucleo.bin" "networkapp@stm32-general.bin"
    "mqttapp@mk3060" "out/mqttapp@mk3060/binary/mqttapp@mk3060.bin" "mqttapp@mk3060-general.bin"
    "mqttapp@esp32devkitc" "out/mqttapp@esp32devkitc/binary/mqttapp@esp32devkitc.bin" "mqttapp@esp32-general.bin"
    "mqttapp@starterkit" "out/mqttapp@starterkit/binary/mqttapp@starterkit.bin" "mqttapp@starterkit-general.bin"
    "mqttapp@developerkit" "out/mqttapp@developerkit/binary/mqttapp@developerkit.bin" "mqttapp@developerkit-general.bin"
    "coapapp@mk3060" "out/coapapp@mk3060/binary/coapapp@mk3060.bin" "coapapp@mk3060-general.bin"
    "coapapp@esp32devkitc" "out/coapapp@esp32devkitc/binary/coapapp@esp32devkitc.bin" "coapapp@esp32-general.bin"
    "acapp@mk3060" "out/acapp@mk3060/binary/acapp@mk3060.bin" "acapp@mk3060-general.bin"
    "acapp@stm32l432kc-nucleo" "out/acapp@stm32l432kc-nucleo/binary/acapp@stm32l432kc-nucleo.bin" "acapp@stm32-general.bin"
    "acapp@esp32devkitc" "out/acapp@esp32devkitc/binary/acapp@esp32devkitc.bin" "acapp@esp32-general.bin"
    "acapp@starterkit" "out/acapp@starterkit/binary/acapp@starterkit.bin" "acapp@starterkit-general.bin"
    "acapp@developerkit" "out/acapp@developerkit/binary/acapp@developerkit.bin" "acapp@developerkit-general.bin"
    "helloworld@mk3060" "out/helloworld@mk3060/binary/helloworld@mk3060.bin" "helloworld@mk3060-general.bin"
    "helloworld@esp32devkitc" "out/helloworld@esp32devkitc/binary/helloworld@esp32devkitc.bin" "helloworld@esp32-general.bin"
    "helloworld@stm32l432kc-nucleo" "out/helloworld@stm32l432kc-nucleo/binary/helloworld@stm32l432kc-nucleo.bin" "helloworld@stm32-general.bin"
    "helloworld@starterkit" "out/helloworld@starterkit/binary/helloworld@starterkit.bin" "helloworld@starterkit-general.bin"
    "helloworld@developerkit" "out/helloworld@developerkit/binary/helloworld@developerkit.bin" "helloworld@developerkit-general.bin"
    "linkkitapp@mk3060 autoywss=1" "out/linkkitapp@mk3060/binary/linkkitapp@mk3060.bin" "linkkitapp@mk3060-autoywss.bin"
    "linkkitapp@mk3060 enrollee=1" "out/linkkitapp@mk3060/binary/linkkitapp@mk3060.bin" "linkkitapp@mk3060-enrollee.bin"
)

#upgrade pip and aos-cube
timeout 120 sudo pip install -U pip > /dev/null 2>&1
timeout 120 sudo pip install -U aos-cube > /dev/null 2>&1

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

arraylength=${#targets[@]}
if [ $((arraylength%3)) -ne 0 ];then
    echo "error: prebuild parameter error"
fi

aos make clean > /dev/null 2>&1
rm -f build.log
for (( i=1; i<${arraylength}+1; i=i+3 ));do
    target=${targets[$i-1]}
    if [ "$(uname)" != "Linux" ] && [[ ${target} == *linuxhost* ]]; then continue; fi

    #modify mqttapp identification info
    if [[ ${target} == *mqttapp@mk3060* ]]; then
        key_store_file="framework/protocol/linkkit/iotkit/sdk-encap/imports/iot_import_product.h"
        sed -i 's/yfTuLfBJTiL/a16GcXEm4uG/g' ${key_store_file}
        sed -i 's/TestDeviceForDemo/mqtt_mk3060_test01/g' ${key_store_file}
        sed -i 's/fSCl9Ns5YPnYN8Ocg0VEel1kXFnRlV6c/wJBI866M9aA22oY97eQnY3fI022rSjOs/g' ${key_store_file}
    elif [[ ${target} == *mqttapp@esp32* ]]; then
        key_store_file="framework/protocol/linkkit/iotkit/sdk-encap/imports/iot_import_product.h"
        sed -i 's/yfTuLfBJTiL/a16GcXEm4uG/g' ${key_store_file}
        sed -i 's/TestDeviceForDemo/mqtt_esp32_test01/g' ${key_store_file}
        sed -i 's/fSCl9Ns5YPnYN8Ocg0VEel1kXFnRlV6c/EY5ZYYOhUAjmz2wnzfIDsNIGcf17RF9H/g' ${key_store_file}
    elif [[ ${target} == *mqttapp@developer* ]]; then
        key_store_file="framework/protocol/linkkit/iotkit/sdk-encap/imports/iot_import_product.h"
        #PRODUCT_KEY,DEVICE_NAME,DEVICE_SECRET
        sed -i 's/yfTuLfBJTiL/b1MRQxodnsW/g' ${key_store_file}
        sed -i 's/TestDeviceForDemo/mqtt_developerkit_test01/g' ${key_store_file}
        sed -i 's/fSCl9Ns5YPnYN8Ocg0VEel1kXFnRlV6c/n6Mf1Qk5mlrRZGqVaOgYqEulvXgJLSGS/g' ${key_store_file}
    elif [[ ${target} == *mqttapp@starter* ]]; then
        key_store_file="framework/protocol/linkkit/iotkit/sdk-encap/imports/iot_import_product.h"
        #PRODUCT_KEY,DEVICE_NAME,DEVICE_SECRET
        sed -i 's/yfTuLfBJTiL/b1bIbpS9H3i/g' ${key_store_file}
        sed -i 's/TestDeviceForDemo/mqtt_starterkit_test01/g' ${key_store_file}
        sed -i 's/fSCl9Ns5YPnYN8Ocg0VEel1kXFnRlV6c/IjiV6l67wLNwTrXqsU4rTWY2MO9F9Pi6/g' ${key_store_file}
    fi

    #modify coapapp identification info
    if [[ ${target} == *coapapp@mk3060* ]]; then
        key_store_file="app/example/coapapp/coapapp.c"
        sed -i 's/fb6pcJ5Z5q5/a16GcXEm4uG/g' ${key_store_file}
        sed -i 's/zt01/coap_mk3060_test01/g' ${key_store_file}
        sed -i 's/PyIjfWwCJZ8YpjjAI5sqATypExx66d4r/u4OcpastxpK91HMN06XrhrZYpNeE3ysq/g' ${key_store_file}
    elif [[ ${target} == *coapapp@esp32* ]]; then
        key_store_file="app/example/coapapp/coapapp.c"
        sed -i 's/fb6pcJ5Z5q5/a16GcXEm4uG/g' ${key_store_file}
        sed -i 's/zt01/coap_esp32_test01/g' ${key_store_file}
        sed -i 's/PyIjfWwCJZ8YpjjAI5sqATypExx66d4r/3yN1CHfTrkfsozHRPHk3WJE6wHOWt4rF/g' ${key_store_file}
    fi

    #modify linkkit identification info
    if [[ ${target} == *linkkitapp@mk3060* ]]; then
        key_store_file="framework/protocol/linkkit/iotkit/sdk-encap/imports/iot_import_product.h"
        if [[ ${target} == *autoywss=1* ]]; then
            sed -i 's/a1AzoSi5TMc/a1P2oHFyPj1/g' ${key_store_file}
            sed -i 's/Z9Ze6qgMrWgTOezW/3SCmc4duJLRxZM68/g' ${key_store_file}
            sed -i 's/test_light_03/VNHU6iQgPaBybZTSYQLN/g' ${key_store_file}
            sed -i 's/oIdAOeech8fM7aHtq0QSvV1oSle30SxP/SsdyJgm7PhYyoMbeBou5Rv9KeqpBBy83/g' ${key_store_file}
        elif [[ ${target} == *enrollee=1* ]]; then
            sed -i 's/a1AzoSi5TMc/a1P2oHFyPj1/g' ${key_store_file}
            sed -i 's/Z9Ze6qgMrWgTOezW/3SCmc4duJLRxZM68/g' ${key_store_file}
            sed -i 's/test_light_03/yo2zSNU51SgbXh9x4y7o/g' ${key_store_file}
            sed -i 's/oIdAOeech8fM7aHtq0QSvV1oSle30SxP/qH5ncnzTXLIpNWx2SQuScibFJyBtfuYG/g' ${key_store_file}
        else
            sed -i 's/a1AzoSi5TMc/a1I6ZYgfWMK/g' ${key_store_file}
            sed -i 's/Z9Ze6qgMrWgTOezW/uhfUXepJkTMIS1Bu/g' ${key_store_file}
            sed -i 's/test_light_03/linkkit_mk3060_test01/g' ${key_store_file}
            sed -i 's/oIdAOeech8fM7aHtq0QSvV1oSle30SxP/qW83MXg4x8FHWQqlseQ844S1DbOQxGm2/g' ${key_store_file}
        fi
        # <TODO> Remove adha discover temperaryly for autoywss and zconfig tests
        if [[ ${target} == *autoywss=1* || ${target} == *enrollee=1* ]]; then
            adha_work_file="framework/ywss4linkkit/libywss4linkkit/awss.c"
            adha_ssid_file="framework/ywss4linkkit/libywss4linkkit/zconfig_protocol.c"
            [ -f ${adha_work_file} ] && sed -i "s/\.name = \"adha\",/\.name = \"excludeadha\",/g" ${adha_work_file}
            [ -f ${adha_ssid_file} ] && sed -i "s/const char \*zc_adha_ssid = \"adha\"/const char \*zc_adha_ssid = \"excludeadha\"/g" ${adha_ssid_file}
        fi
        sed -i 's/AOS_LL_DEBUG/AOS_LL_INFO/g' app/example/linkkitapp/linkkit-example.c
    elif [[ ${target} == *linkkitapp@esp32* ]]; then
        key_store_file="framework/protocol/linkkit/iotkit/sdk-encap/imports/iot_import_product.h"
        sed -i 's/a1AzoSi5TMc/a1I6ZYgfWMK/g' ${key_store_file}
        sed -i 's/Z9Ze6qgMrWgTOezW/uhfUXepJkTMIS1Bu/g' ${key_store_file}
        sed -i 's/test_light_03/linkkit_esp32_test01/g' ${key_store_file}
        sed -i 's/oIdAOeech8fM7aHtq0QSvV1oSle30SxP/qqJtZhrY6ApGMoO74dr6Bw0QdLSSb2Ok/g' ${key_store_file}
        sed -i 's/AOS_LL_DEBUG/AOS_LL_INFO/g' app/example/linkkitapp/linkkit-example.c
    fi

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

    if [[ ${target} == *coapapp* ]] || [[ ${target} == *mqttapp* ]]; then
        git checkout ${key_store_file}
    fi

    if [[ ${target} == *linkkitapp* ]]; then
        git checkout ${key_store_file}
        git checkout app/example/linkkitapp/linkkit-example.c
        if [[ ${target} == *autoywss=1* || ${target} == *enrollee=1* ]]; then
            git checkout ${adha_work_file}
            git checkout ${adha_ssid_file}
        fi
    fi
done
