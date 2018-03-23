src = Split('''
    cjson_test.c
''')

component = aos_component('cjson_test', src)

component.add_component_dependencis('utility/cjson')

component.add_cflags('-Wall')
component.add_cflags('-Werror')