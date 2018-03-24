src = Split('''
    mqtt-example.c
''')
component = aos_component('mqttapp', src)

macro_tmp = Split('''
    ALIOT_DEBUG 
    IOTX_DEBUG
''')
for i in macro_tmp:
    component.add_global_macro(i)

dependencis = Split('''
    tools/cli 
    framework/connectivity/mqtt
    utility/cjson 
    framework/fota 
    framework/netmgr 
    framework/common
''')
for i in dependencis:
    component.add_component_dependencis(i)