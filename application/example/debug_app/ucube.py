src     = Split('''
        debug_app.c
''')

component = aos_component('debug_app', src)
component.add_comp_deps('kernel/debug', 'kernel/cli')
