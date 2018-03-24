src     = Split('''
        sal_sockets.c 
        sal_err.c 
        sal_arch.c 
        ip4_addr.c 
        sal.c
        sal_device.c
''')

component = aos_component('sal', src)

component.add_component_dependencis('device/sal/wifi/mk3060')

component.add_global_includes('include')

component.add_global_macro('WITH_SAL')
