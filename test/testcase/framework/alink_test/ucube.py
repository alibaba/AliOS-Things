src = Split('''
    alink_test.c
''')

component = aos_component('alink_test', src)

component.add_comp_deps('middleware/alink/protocol')
component.add_comp_deps('middleware/alink/ywss')

component.add_cflags('-Wall')
component.add_cflags('-Werror')