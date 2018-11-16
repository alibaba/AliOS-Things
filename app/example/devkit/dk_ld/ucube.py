src = Split('''
    mqtt-app.app/example.c
''')
component = aos_component('ldapp', src)
global_includes =Split('''
    ../../../device/sensor/include
''')

macro_tmp = Split('''
    ALIOT_DEBUG
    IOTX_DEBUG
    USE_LPTHREAD
''')
for i in macro_tmp:
    component.add_global_macros(i)

dependencis = Split('''
    tools/cli
    connectivity/mqtt
    utility/cjson
    middleware/uagent/uota
    network/netmgr
    middleware/common
''')
for i in dependencis:
    component.add_comp_deps(i)
