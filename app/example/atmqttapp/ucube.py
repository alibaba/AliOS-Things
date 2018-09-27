src     = Split('''
        atmqttapp.c
''')

component = aos_component('atmqttapp', src)
component.add_comp_deps('kernel/yloop', 'tools/cli')
component.add_global_macros('AOS_NO_WIFI')
