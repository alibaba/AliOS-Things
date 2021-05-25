src = Split('''
    rtc_test.c
''')

component = aos_component('rtc_test', src)

component.add_cflags('-Wall')
component.add_cflags('-Werror')
