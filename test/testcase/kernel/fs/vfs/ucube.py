src = Split('''
    vfs_test.c
''')

component = aos_component('vfs_test', src)

component.add_comp_deps('kernel/fs/vfs')

component.add_cflags('-Wall')
component.add_cflags('-Werror')
