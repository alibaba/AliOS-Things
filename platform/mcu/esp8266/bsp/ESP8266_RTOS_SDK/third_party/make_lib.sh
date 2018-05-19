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

cd $1
make clean
make
cp .output/eagle/debug/lib/lib$1.a $SDK_PATH/lib/lib$1.a
xtensa-lx106-elf-strip --strip-unneeded $SDK_PATH/lib/lib$1.a
cd ..
