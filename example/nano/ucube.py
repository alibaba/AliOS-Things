src = Split('''
    nano.c
''')

component = aos_component('nano', src)

component.add_global_macro('AOS_NO_WIFI')
component.add_global_macro('CONFIG_NO_TCPIP')

aos_global_config.set_aos_global_config('mesh','0')
