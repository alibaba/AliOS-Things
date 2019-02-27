src =Split(''' 
    app_entry.c
    coap-example.c
''')

component =aos_component('coapapp', src)

dependencis =Split(''' 
    framework/protocol/linkkit/sdk
    framework/protocol/linkkit/hal
    framework/netmgr
    framework/common
    utility/cjson
    kernel/cli
''')
for i in dependencis:
    component.add_comp_deps(i)

global_macros =Split(''' 
    MQTT_DIRECT
    ALIOT_DEBUG
    IOTX_DEBUG
    USE_LPTHREAD
''')
for i in global_macros:
    component.add_global_macros(i)
