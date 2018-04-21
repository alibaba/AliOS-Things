src = Split('''
    cloud.c
''')

component = aos_component('cloud', src)
component.add_global_macros('CONFIG_CLOUD')