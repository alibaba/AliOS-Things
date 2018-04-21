src = Split('''
    lite-log.c
''')

component = aos_component('LITE-log', src)

component.add_macros('DEBUG')
component.add_includes('.')