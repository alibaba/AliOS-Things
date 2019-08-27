src = Split('''
    ulocation_test.c
''')

component = aos_component('ulocation_test', src)

component.add_comp_deps('middleware/ulocation')

component.add_cflags('-Wall')
component.add_cflags('-Werror')