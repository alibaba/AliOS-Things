src = []

vcall_macro = ''
component_dependencis = []
vcall = aos_global_config.get_aos_global_config('vcall')

if vcall == 'freertos':
    vcall_macro = 'VCALL_FREERTOS'
    src.append('aos/aos_freertos.c')
elif vcall == 'posix':
    vcall_macro = VCALL_POSIX
    src.append('aos/aos_posix.c.c')
else:
    vcall_macro = 'VCALL_RHINO'
    component_dependencis.append('kernel/rhino')

    if aos_global_config.mcu_family == 'esp32' or aos_global_config.mcu_family == 'esp8266':
        component_dependencis.append('kernel/vcall/espos')

    if aos_global_config.board == 'linuxhost' or aos_global_config.board == 'mk3060' \
            or aos_global_config.board == 'mk3239' or aos_global_config.board == 'mk3166':
        src.append('mico/mico_rhino.c')
    src.append('aos/aos_rhino.c')

component = aos_component('vcall', src)
component.add_global_includes('mico/include')

component.add_global_macro(vcall_macro)
component.add_component_dependencis(*component_dependencis)

if aos_global_config.arch == 'ARM968E-S':
    component.add_cflags('-marm')