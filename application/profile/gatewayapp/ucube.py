src =Split('''
    udata_example.c
''')

component =aos_component('udataapp', src)
global_includes =Split('''
    .
    ../../../drivers/sensor/include
''')

dependencis =Split('''
    kernel/cli
    network/netmgr
    drivers/sensor
    middleware/udata
''')
for i in dependencis:
    component.add_comp_deps(i)

global_macros =Split('''
''')
for i in global_macros:
    component.add_global_macros(i)

