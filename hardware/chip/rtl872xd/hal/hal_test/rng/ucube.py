src = Split('''
    rng_test.c
''')

component = aos_component('rng_test', src)

component.add_cflags('-Wall')
component.add_cflags('-Werror')
