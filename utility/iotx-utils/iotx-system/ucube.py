src = Split('''
    device.c
    ca.c
''')

component = aos_component('libiotx-system', src)
component.add_global_includes('.')