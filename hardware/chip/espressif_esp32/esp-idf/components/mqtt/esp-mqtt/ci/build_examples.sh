#!/usr/bin/env bash

# build mqtt examples with make if $1=="make", with cmake otherwise
set -o errexit # Exit if command failed.

if [ -z $IDF_PATH ] ; then
    echo "Mandatory variables undefined"
    exit 1;
fi;

examples="tcp ssl ssl_mutual_auth ws wss"
for i in $examples; do
    echo "Building MQTT example $i"
    cd $IDF_PATH/examples/protocols/mqtt/$i
    if [[ "$1" = "make" ]]; then
        make defconfig
        make -j 4
    else
        rm -rf build sdkconfig
        idf.py build
    fi;
done
