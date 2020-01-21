src     = Split('''
        halapp.c
''')

component = aos_component('halapp', src)
component.add_comp_deps('kernel/yloop', 'kernel/cli')
component.add_global_macros('AOS_NO_WIFI')
