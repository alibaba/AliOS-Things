src     = Split('''
        helloworld.c
''')

component = aos_component('helloworld', src)
component.add_comp_deps('kernel/yloop', 'tools/cli')
component.add_global_macros('AOS_NO_WIFI')
