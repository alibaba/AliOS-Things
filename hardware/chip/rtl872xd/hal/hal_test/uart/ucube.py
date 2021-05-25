src = Split('''
    uart_test.c
''')

component = aos_component('uart_test', src)

component.add_cflags('-Wall')
component.add_cflags('-Werror')
