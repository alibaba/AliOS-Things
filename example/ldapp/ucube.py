src = Split('''
    mqtt-example.c
''')
component = aos_component('ldapp', src)

macro_tmp = Split('''
    ALIOT_DEBUG
    IOTX_DEBUG
''')
for i in macro_tmp:
    component.add_global_macros(i)

dependencis = Split('''
    tools/cli
    framework/connectivity/mqtt
    utility/cjson
    framework/uOTA
    framework/netmgr
    framework/common
''')
for i in dependencis:
    component.add_comp_deps(i)
