src     = Split('''
        footprint_test.c
''')

component = aos_component('footprint_test', src)
component.add_comp_deps('kernel/yloop', 'tools/cli')
component.add_global_macros('AOS_NO_WIFI')
component.add_global_macros('RHINO_CONFIG_SCHED_STATS=1')
component.add_global_macros('RHINO_CONFIG_REALTIME_TEST=1')
