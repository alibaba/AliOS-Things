src = Split('''
    alink_test.c
''')

component = aos_component('alink_test', src)

component.add_component_dependencis('framework/protocol/alink')
component.add_component_dependencis('framework/ywss')

component.add_cflags('-Wall')
component.add_cflags('-Werror')