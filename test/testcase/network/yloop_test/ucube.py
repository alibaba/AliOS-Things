src = Split('''
    yloop_test.c
''')

component = aos_component('yloop_test', src)

component.add_comp_deps('kernel/yloop')

component.add_cflags('-Wall')
component.add_cflags('-Werror')