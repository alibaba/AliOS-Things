src =Split('''
    udata_example.c
''')

component =aos_component('udataapp', src)
global_includes =Split(''' 
    .
    ../../../device/sensor/include
''')

dependencis =Split('''
    kernel/cli
    network/netmgr
    middleware/common
    device/sensor 
    middleware/udata
''')
for i in dependencis:
    component.add_comp_deps(i)

global_macros =Split('''
    CONFIG_AOS_CLI
''')
for i in global_macros:
    component.add_global_macros(i)

