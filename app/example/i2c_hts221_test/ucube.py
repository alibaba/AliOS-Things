src     = Split('''
        i2c_hts221_test.c
''')

component = aos_component('i2c_hts221_test', src)
component.add_comp_deps('kernel/yloop', 'tools/cli')
component.add_global_macros('AOS_NO_WIFI')

dependencis =Split(''' 
    tools/cli
    device/sensor 
    middleware/common
    middleware/udata
    network/netmgr
''')
for i in dependencis:
    component.add_comp_deps(i)

global_macros =Split(''' 
    MQTT_DIRECT
    ALIOT_DEBUG
    IOTX_DEBUG
    USE_LPTHREAD
    FOTA_RAM_LIMIT_MODE
    COAP_WITH_YLOOP
    TEST_ALCS
    CONFIG_AOS_CLI
    ON_PRE2=1
''')
for i in global_macros:
    component.add_global_macros(i)

