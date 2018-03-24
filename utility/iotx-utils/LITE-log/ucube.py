src = Split('''
    lite-log.c
''')

component = aos_component('LITE-log', src)

component.add_macro('DEBUG')
component.add_includes('.')