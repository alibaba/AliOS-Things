src     = Split('''
        main.c
''')

component = aos_component('appdemo', src)
component.add_global_macros('AOS_NO_WIFI')
