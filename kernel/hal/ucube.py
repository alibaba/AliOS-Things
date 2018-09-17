src     = Split('''
        wifi.c
        ota.c					
''')
component = aos_component('hal', src)

component.add_global_macros('AOS_HAL')
component.add_global_includes('include', '../rhino', '../rhino/hal/soc')
