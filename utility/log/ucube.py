src     = Split('''
        log.c
''')
component = aos_component('log', src)

component.add_global_includes('include')
