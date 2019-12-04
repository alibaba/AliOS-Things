src = Split('''
    netmgr_yts_test.c
''')

component = aos_component('netmgr_yts_test', src)

component.add_comp_deps('network/netmgr')

component.add_cflags('-Wall')
component.add_cflags('-Werror')
