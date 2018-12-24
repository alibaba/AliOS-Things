src     = Split('''
        modbus_test.c
        modbus_demo.c
''')

component = aos_component('modbus_test', src)
component.add_comp_deps('kernel/yloop', 'kernel/cli','kernel/rhino/bus/mbmaster')
component.add_global_macros('AOS_NO_WIFI')
