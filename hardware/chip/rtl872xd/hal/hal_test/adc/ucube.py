src = Split('''
    adc_test.c
''')

component = aos_component('adc_test', src)

component.add_cflags('-Wall')
component.add_cflags('-Werror')
