src = Split('''
    wifi_atcmd_bk7231.c
    bk7231.c
''')

component = aos_component('device_sal_bk7231', src)

component.add_comp_deps('network/nal/at')
component.add_comp_deps('kernel/yloop')

component.add_global_includes('.')

component.add_global_macros('DEV_SAL_BK7231')