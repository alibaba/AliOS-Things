src =Split(''' 
    uData-example.c
    uData_app.c
    linkkit_export.c
    lite_queue.c
    uData_tsl.c
''')
component =aos_component('uDataapp', src)

global_includes =Split(''' 
    .
    ../../../device/sensor/include
''')

dependencis =Split(''' 
    connectivity/mqtt
    middleware/uagent/uota
    network/netmgr
    middleware/common
    middleware/linkkit/cm
    middleware/linkkit/dm
    middleware/linkkit/alcs
    middleware/linkkit/iotkit
    middleware/linkkit/ywss
    tools/cli
    device/sensor 
    middleware/udata
''')
for i in dependencis:
    component.add_comp_deps(i)

global_macros =Split(''' 
    MQTT_DIRECT
    ALIOT_DEBUG
    IOTX_DEBUG
    USE_LPTHREAD
    UOTA_RAM_LIMIT_MODE
    COAP_WITH_YLOOP
    TEST_ALCS
    CONFIG_AOS_CLI
    ON_PRE2=1
    DATA_TO_CLOUD
''')
for i in global_macros:
    component.add_global_macros(i)

if aos_global_config.get('LWIP') == 1:
    component.add_comp_deps("network/lwip")
    aos_global_config.set('no_with_lwip', 0)

