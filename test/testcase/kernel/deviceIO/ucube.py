src = Split('''
    deviceIO_test.c
''')

component = aos_component('deviceIO_test', src)

component.add_comp_deps('kernel/vfs')

component.add_cflags('-Wall')
component.add_cflags('-Werror')