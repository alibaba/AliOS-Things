src = Split('''
    cjson_test.c
''')

component = aos_component('cjson_test', src)

component.add_comp_deps('utility/cjson')

component.add_cflags('-Wall')
component.add_cflags('-Werror')