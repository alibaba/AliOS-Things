src     = Split('''
        coapapp.c
''')

component = aos_component('coapapp', src)
component.add_comp_deps('kernel/yloop', 'tools/cli','connectivity/coap','netmgr')
component.add_global_macros('AOS_NO_WIFI')
