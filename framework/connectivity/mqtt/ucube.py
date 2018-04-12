src =Split(''' 
    mqtt_client.c
    mqtt_instance.c
''')
component =aos_component('mqtt', src)

dependencis =Split(''' 
    framework/connectivity/mqtt/MQTTPacket
    framework/protocol/alink-ilop
''')
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split(''' 
    ./
    ../../protocol/alink-ilop/sdk-encap
    ../../protocol/alink-ilop/base/utils
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
    ../../protocol/alink-ilop/iotkit-system
    ../../protocol/alink-ilop/base/log/LITE-log
    ../../protocol/alink-ilop/base/utils/LITE-utils/src
    ../../protocol/alink-ilop/base/utils/misc/
    ../../protocol/alink-ilop/base/utils/digest
''')
for i in includes:
    component.add_includes(i)


cflags =Split(''' 
    -DOTA_SIGNAL_CHANNEL=1
''')
for i in cflags:
    component.add_cflags(i)
