src     = Split('''
        kernel_test.c
''')

component = aos_component('kernel_test', src)
component.add_comp_deps('kernel/cli', 'kernel/rhino/test')
component.add_global_macros('AOS_NO_WIFI')
