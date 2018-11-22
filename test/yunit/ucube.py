src = Split('''
    yunit.c
''')

component = aos_component('yunit', src)
component.add_global_includes('include')

