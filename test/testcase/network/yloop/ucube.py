src = Split('''
    yloop_yts_test.c
''')

component = aos_component('yloop_yts_test', src)

component.add_comp_deps('network/yloop')

component.add_cflags('-Wall')
component.add_cflags('-Werror')
