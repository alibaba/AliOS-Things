src = Split('''
    cloud.c
''')

component = aos_component('cloud', src)
component.add_global_macro('CONFIG_CLOUD')