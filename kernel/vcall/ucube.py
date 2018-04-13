component = aos_component('vcall', [])

@pre_config('vcall')
def pre_vacll(component):
    vcall = aos_global_config.get('vcall', 'rhino')
    if vcall == 'freertos':
        component.add_global_macros('VCALL_FREERTOS')
        component.add_sources('aos/aos_freertos.c')
    elif vcall == 'posix':
        component.add_global_macros('VCALL_POSIX')
        component.add_sources('aos/aos_posix.c')
    else:
        component.add_global_macros('VCALL_RHINO')
        component.add_comp_deps('kernel/rhino')

        if aos_global_config.mcu_family == 'esp32' or aos_global_config.mcu_family == 'esp8266':
            component.add_comp_deps('kernel/vcall/espos')

        if aos_global_config.board == 'linuxhost' or aos_global_config.board == 'mk3060' \
                or aos_global_config.board == 'mk3239' or aos_global_config.board == 'mk3166' or aos_global_config.board == 'hobbit1_evb':
            component.add_sources('mico/mico_rhino.c')
        component.add_sources('aos/aos_rhino.c')


pre_vacll(component)

component.add_global_includes('mico/include')

if aos_global_config.arch == 'ARM968E-S':
    component.add_cflags('-marm')