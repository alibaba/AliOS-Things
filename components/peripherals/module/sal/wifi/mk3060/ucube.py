src = Split('''
    wifi_atcmd.c
    mk3060.c
''')

component = aos_component('device_sal_mk3060', src)

component.add_comp_deps('network/nal/at')
component.add_comp_deps('kernel/yloop')

component.add_global_includes('.')

component.add_global_macros('DEV_SAL_MK3060')