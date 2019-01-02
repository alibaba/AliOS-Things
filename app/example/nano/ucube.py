src = Split('''
    nano.c
''')

component = aos_component('nano', src)

component.add_global_macros('AOS_NO_WIFI')
component.add_global_macros('CONFIG_NO_TCPIP')

aos_global_config.set('mesh','0')
