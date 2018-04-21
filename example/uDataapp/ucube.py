src =Split(''' 
    uData_sample.c
''')
component =aos_component('uDataapp', src)

dependencis =Split(''' 
    tools/cli
    framework/netmgr
    framework/common
    device/sensor
    framework/uData
''')
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split(''' 
    .
''')
for i in global_includes:
    component.add_global_includes(i)


includes =Split(''' 
    ./include
    ../../include/aos
    ../../include/hal
''')
for i in includes:
    component.add_includes(i)


