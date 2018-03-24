#!/bin/bash
set -e

export SDK_PATH=$(dirname $(pwd))

echo "make_lib.sh version 20150924"
echo ""

if [ $SDK_PATH ]; then
    echo "SDK_PATH:"
    echo "$SDK_PATH"
    echo ""
else
    echo "ERROR: Please export SDK_PATH in make_lib.sh firstly, exit!!!"
    exit
fi

for dir in freertos json lwip mbedtls nopoll openssl spiffs ssl; do
    cd $dir
    make clean
    make
    cp .output/eagle/debug/lib/lib$dir.a $SDK_PATH/lib/lib$dir.a
    xtensa-lx106-elf-strip --strip-unneeded $SDK_PATH/lib/lib$dir.a
    cd ..
done;