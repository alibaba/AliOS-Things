src     = Split('''
        lwm2m_example.c
''')

component = aos_component('lwm2mapp', src)
component.add_comp_deps('kernel/yloop', 'kernel/cli')
component.add_global_macros('AOS_NO_WIFI', '')
