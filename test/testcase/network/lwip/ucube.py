src = Split('''
    lwip_test.c
''')

component = aos_component('lwip_test', src)

component.add_comp_deps('network/lwip')

component.add_cflags('-Wall')
component.add_cflags('-Werror')
