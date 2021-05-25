src = Split('''
    i2c_test.c
''')

component = aos_component('i2c_test', src)

component.add_cflags('-Wall')
component.add_cflags('-Werror')
