src     = Split('''
        pwr_test.c
''')

component = aos_component('pwr_test', src)
component.add_comp_deps('kernel/yloop', 'tools/cli','rhino/pwrmgmt')
component.add_global_macros('AOS_NO_WIFI')
