src = Split('''
    dac_test.c
''')

component = aos_component('dac_test', src)

component.add_cflags('-Wall')
component.add_cflags('-Werror')
