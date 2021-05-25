src = Split('''
    spi_test.c
''')

component = aos_component('spi_test', src)

component.add_cflags('-Wall')
component.add_cflags('-Werror')
