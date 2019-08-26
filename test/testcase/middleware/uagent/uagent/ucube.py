src = Split('''
    uagent_test.c
''')

component = aos_component('uagent_test', src)

component.add_comp_deps('middleware/uagent/uagent')

component.add_cflags('-Wall')
component.add_cflags('-Werror')
