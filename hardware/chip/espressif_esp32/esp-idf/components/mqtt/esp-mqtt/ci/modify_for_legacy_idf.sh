#!/usr/bin/env bash

if [[ -z $1 ]]; then
    LATEST_IDF=master
else
    LATEST_IDF=$1
fi

# This snipped prepares environment for using esp-mqtt repository separately from idf -- legacy use before IDFv3.2
#
esp_mqtt_path=`pwd`
mkdir -p ${esp_mqtt_path}/examples
pushd
cd $IDF_PATH
former_commit_id=`git rev-parse HEAD`
git checkout ${LATEST_IDF}

for example in tcp; do
    cp -r $IDF_PATH/examples/protocols/mqtt/${example} ${esp_mqtt_path}/examples
    echo 'EXTRA_COMPONENT_DIRS += $(PROJECT_PATH)/../../../' >   ${esp_mqtt_path}/examples/${example}/Makefile
    cat $IDF_PATH/examples/protocols/mqtt/${example}/Makefile >> ${esp_mqtt_path}/examples/${example}/Makefile
    echo "CONFIG_MQTT_TRANSPORT_SSL=" >> ${esp_mqtt_path}/examples/${example}/sdkconfig.defaults
    echo "CONFIG_MQTT_TRANSPORT_WEBSOCKET=" >> ${esp_mqtt_path}/examples/${example}/sdkconfig.defaults
done

cp -r $IDF_PATH/components/tcp_transport ${esp_mqtt_path}/..
rm    ${esp_mqtt_path}/../tcp_transport/transport_ssl.c
echo -e "#include \"esp_transport.h\"\nvoid esp_transport_ws_set_path(esp_transport_handle_t t, const char *path) {}" > ${esp_mqtt_path}/../tcp_transport/transport_ws.c

cp $IDF_PATH/components/mqtt/Kconfig  ${esp_mqtt_path}
sed 's/esp-mqtt/\./g' $IDF_PATH/components/mqtt/component.mk  > ${esp_mqtt_path}/component.mk
git checkout $former_commit_id
popd