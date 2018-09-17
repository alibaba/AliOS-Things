src =Split(''' 
    app_entry.c
    mqtt-example.c
''')

component =aos_component('mqttapp', src)

dependencis =Split(''' 
    framework/protocol/linkkit/sdk
    framework/protocol/linkkit/hal
    framework/netmgr
    framework/common
    utility/cjson
    tools/cli
''')
for i in dependencis:
    component.add_comp_deps(i)

global_macros =Split(''' 
    MQTT_DIRECT
    ALIOT_DEBUG
    IOTX_DEBUG
    USE_LPTHREAD
    CONFIG_AOS_CLI
''')
for i in global_macros:
    component.add_global_macros(i)

if aos_global_config.get('LWIP') == 1:
    component.add_comp_deps("kernel/protocols/net")
    aos_global_config.set('no_with_lwip', 0)

