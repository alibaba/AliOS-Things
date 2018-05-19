src = Split('''
    vcall_test.c
''')

component = aos_component('vcall_test', src)

component.add_comp_deps('kernel/vcall')

component.add_cflags('-Wall')
component.add_cflags('-Werror')