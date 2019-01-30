src =Split(''"
''')
component =aos_component('device_sal_esp8266_aos', src)

dependencis =Split('''
    kernel/yloop
''')
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split('''
    ./
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split('''
    DEV_SAL_ESP8266_AOS
''')
for i in global_macros:
    component.add_global_macros(i)

if at_adapter == 1:
    component.add_comp_deps('network/sal')
    src.add_sources('esp8266.c')
