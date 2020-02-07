src     = Split('''
        kernel_app.c
        task_app.c
''')

component = aos_component('kernel_app', src)
component.add_comp_deps('kernel/cli')
component.add_global_macros('AOS_NO_WIFI')
