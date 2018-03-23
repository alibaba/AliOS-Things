src     = Split('''
        wifi.c
        ota.c					
''')
component = aos_component('hal', src)

component.add_global_macro('AOS_HAL')
