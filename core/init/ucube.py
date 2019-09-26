src     = Split('''
        aos_init.c
''')
component = aos_component('kernel_init', src)


component.add_global_includes('include')
