src = Split('''
    gpio_test.c
''')

component = aos_component('gpio_test', src)

component.add_cflags('-Wall')
component.add_cflags('-Werror')
