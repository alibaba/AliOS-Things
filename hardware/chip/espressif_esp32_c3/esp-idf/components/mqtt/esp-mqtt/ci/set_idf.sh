#!/usr/bin/env bash

# sets up the IDF repo incl submodules with specified version as $1
set -o errexit # Exit if command failed.

if [ -z $IDF_PATH ] || [ -z $MQTT_PATH ] || [ -z $1 ] ; then
    echo "Mandatory variables undefined"
    exit 1;
fi;

echo "Checking out IDF version $1"
cd $IDF_PATH
# Cleans out the untracked files in the repo, so the next "git checkout" doesn't fail
git clean -f
git checkout $1
# Removes the mqtt submodule, so the next submodule update doesn't fail
rm -rf $IDF_PATH/components/mqtt/esp-mqtt
git submodule update --init --recursive

