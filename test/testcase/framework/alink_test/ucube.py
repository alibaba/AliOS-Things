src = Split('''
    alink_test.c
''')

component = aos_component('alink_test', src)

component.add_comp_deps('framework/protocol/alink')
component.add_comp_deps('framework/ywss')

component.add_cflags('-Wall')
component.add_cflags('-Werror')