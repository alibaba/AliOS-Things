src = Split('''
    vfs_test.c
''')

component = aos_component('vfs_test', src)

component.add_component_dependencis('kernel/vfs')

component.add_cflags('-Wall')
component.add_cflags('-Werror')