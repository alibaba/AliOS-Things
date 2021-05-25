src = Split('''
    flash_test.c
''')

component = aos_component('flash_test', src)

component.add_cflags('-Wall')
component.add_cflags('-Werror')
