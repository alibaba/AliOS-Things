#!/usr/bin/env bash

# sets the mqtt in IDF tree as a submodule with the version specified as $1
set -o errexit # Exit if command failed.

if [ -z $IDF_PATH ] || [ -z $MQTT_PATH ] || [ -z $1 ] ; then
    echo "Mandatory variables undefined"
    exit 1;
fi;

echo "Checking out MQTT version to $1"
# exchange remotes of mqtt submodules with plain copy
cd $IDF_PATH/components/mqtt/esp-mqtt
rm -rf .git                # removes the actual IDF referenced version
cp -r $MQTT_PATH/.git .    # replaces with the MQTT_PATH (CI checked tree)
git reset --hard $1        # sets the requested version
