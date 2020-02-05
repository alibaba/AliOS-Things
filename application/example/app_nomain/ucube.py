src     = Split('''
        main.c
''')

component = aos_component('app_nomain', src)
component.add_global_macros('AOS_NO_WIFI')
