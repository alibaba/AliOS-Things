src =Split(''' 
    mqtt_client.c
    mqtt_instance.c
''')
component =aos_component('mqtt', src)

dependencis =Split(''' 
    framework/connectivity/mqtt/MQTTPacket
    framework/protocol/linkkit/iotkit
''')
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split(''' 
    ./
    ../../protocol/linkkit/iotkit/sdk-encap
    ../../protocol/linkkit/iotkit/base/utils
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    MQTT_COMM_ENABLED
    CMP_VIA_MQTT_DIRECT
    MQTT_DIRECT
''')
for i in global_macros:
    component.add_global_macros(i)

includes =Split(''' 
    ../../protocol/linkkit/iotkit/iotkit-system
    ../../protocol/linkkit/iotkit/base/log/LITE-log
    ../../protocol/linkkit/iotkit/base/utils/LITE-utils/src
    ../../protocol/linkkit/iotkit/base/utils/misc/
    ../../protocol/linkkit/iotkit/base/utils/digest
''')
for i in includes:
    component.add_includes(i)


cflags =Split(''' 
    -DOTA_SIGNAL_CHANNEL=1
''')
for i in cflags:
    component.add_cflags(i)
