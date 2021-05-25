src = Split('''
    hal_test.c
''')

component = aos_component('hal_test', src)

component.add_cflags('-Wall')
component.add_cflags('-Werror')
