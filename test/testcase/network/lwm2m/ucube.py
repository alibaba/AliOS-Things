src = Split('''
    lwm2m_yts_test.c
''')

component = aos_component('lwm2m_yts_test', src)

component.add_comp_deps('network/lwm2m')

component.add_cflags('-Wall')
component.add_cflags('-Werror')
