src     = Split('''
        helloworld_nocli.c
''')

component = aos_component('helloworld_nocli', src)
component.add_comp_deps('kernel/yloop')
component.add_global_macros('AOS_NO_WIFI')
