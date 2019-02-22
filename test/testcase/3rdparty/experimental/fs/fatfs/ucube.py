src = Split('''
    fatfs_test.c
''')

component = aos_component('fatfs_test', src)

component.add_comp_deps('3rdparty/experimental/fs/fatfs')

component.add_cflags('-Wall')
component.add_cflags('-Werror')
