src = Split('''
    kv_test.c
''')

component = aos_component('kv_test', src)

component.add_comp_deps('kernel/modules/fs/kv')

component.add_cflags('-Wall')
component.add_cflags('-Werror')