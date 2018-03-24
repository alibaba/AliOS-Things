src = Split('''
    wifi_atcmd.c
    mk3060.c
''')

component = aos_component('device_sal_mk3060', src)

component.add_component_dependencis('framework/atparser')
component.add_component_dependencis('kernel/yloop')

component.add_global_includes('.')

component.add_global_macro('DEV_SAL_MK3060')