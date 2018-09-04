src = Split('''
    wifi_atcmd.c
    athost.c
''')

component = aos_component('device_sal_athost', src)

component.add_comp_deps('network/sal/atparser')
component.add_comp_deps('kernel/yloop')

component.add_global_includes('.')

component.add_global_macros('DEV_SAL_ATHOST')
