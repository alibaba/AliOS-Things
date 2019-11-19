src = Split('''
    websoc_yts_test.c
''')

component = aos_component('websoc_yts_test', src)

component.add_comp_deps('network/websoc')

component.add_cflags('-Wall')
component.add_cflags('-Werror')
