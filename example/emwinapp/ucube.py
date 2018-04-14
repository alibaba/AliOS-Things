src     = Split('''
        emwinapp.c
''')

component = aos_component('emwinapp', src)
component.add_comp_deps('kernel/yloop', 'tools/cli')
component.add_global_macros('AOS_NO_WIFI')
