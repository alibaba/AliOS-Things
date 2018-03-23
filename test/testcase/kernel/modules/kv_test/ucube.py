src = Split('''
    kv_test.c
''')

component = aos_component('kv_test', src)

component.add_component_dependencis('kernel/modules/fs/kv')

component.add_cflags('-Wall')
component.add_cflags('-Werror')