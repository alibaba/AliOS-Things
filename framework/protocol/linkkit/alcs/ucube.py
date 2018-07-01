src =Split(''' 
    src/alcs_adapter.c
    src/alcs_mqtt.c
    src/alcs_api.c
    src/alcs_client.c
    src/alcs_coap.c
    src/alcs_server.c
    src/linked_list.c
''')
component =aos_component('alcs', src)

dependencis =Split(''' 
    framework/common
    framework/connectivity/link-coap
    framework/protocol/linkkit/iotkit
''')
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split(''' 
    ./
    ./inc
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    MQTT_DIRECT
    MQTT_COMM_ENABLED
    CMP_VIA_MQTT_DIRECT
    OTA_SIGNAL_CHANNEL=1
    CMP_SUPPORT_MEMORY_MAGIC
''')
for i in global_macros:
    component.add_global_macros(i)

includes =Split(''' 
    ../../../connectivity/link-coap/src
    ../../linkkit/iotkit/sdk-encap
    ../../linkkit/iotkit/iotkit-system
''')
for i in includes:
    component.add_includes(i)

