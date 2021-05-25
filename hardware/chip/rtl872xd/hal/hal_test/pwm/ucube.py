src = Split('''
    pwm_test.c
''')

component = aos_component('pwm_test', src)

component.add_cflags('-Wall')
component.add_cflags('-Werror')
