src = Split('''
    mqtt-example.c
''')
component = aos_component('mqttapp', src)

macro_tmp = Split('''
    MQTT_TEST
    ALIOT_DEBUG 
    IOTX_DEBUG
    USE_LPTHREAD
''')
for i in macro_tmp:
    component.add_global_macros(i)

dependencis = Split('''
    tools/cli 
    framework/connectivity/mqtt
    utility/cjson 
    framework/fota 
    framework/netmgr 
    framework/common
''')
for i in dependencis:
    component.add_comp_deps(i)