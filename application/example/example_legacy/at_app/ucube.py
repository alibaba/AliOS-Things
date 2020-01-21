src     = Split('''
        at_app.c
''')

component = aos_component('at_app', src)
component.add_comp_deps('kernel/yloop', 'kernel/cli')
component.add_global_macros('AOS_NO_WIFI')
