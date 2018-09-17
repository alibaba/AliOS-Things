src =[]
component = aos_component('sal', src)

if aos_global_config.get('at_adapter') != 1:
    src = Split('''
            sal_sockets.c 
            sal_err.c 
            sal_arch.c 
            ip4_addr.c 
            sal.c
            sal_device.c
    ''')
    component.add_sources(*src)
    component.add_global_macros('WITH_SAL')
    component.add_global_includes('include')

if aos_global_config.get('module') == 'wifi.gt202':
    component.add_comp_deps('network/sal/wifi/gt202')
elif aos_global_config.get('module') == 'wifi.mk3060':
    component.add_comp_deps('network/sal/wifi/mk3060')
elif aos_global_config.get('module') == 'wifi.bk7231':
    component.add_comp_deps('network/sal/wifi/bk7231')
elif aos_global_config.get('module') == 'gprs.sim800':
    component.add_comp_deps('network/sal/gprs/sim800')
