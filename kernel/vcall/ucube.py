src = []

component = aos_component('vcall', src)
component.add_global_includes('mico/include')

if aos_global_config.arch == 'ARM968E-S':
    component.add_cflags('-marm')

@pre_config('vcall')
def vcall_pre(comp):
    vcall = aos_global_config.get('vcall', 'rhino')
    if vcall == 'freertos':
        comp.add_global_macros('VCALL_FREERTOS')
        comp.add_sources('aos/aos_freertos.c')
    elif vcall == 'posix':
        comp.add_global_macros('VCALL_POSIX')
        comp.add_sources('aos/aos_posix.c')
    else:
        comp.add_global_macros('VCALL_RHINO')
        comp.add_comp_deps('kernel/rhino')

        if aos_global_config.mcu_family == 'esp32' or aos_global_config.mcu_family == 'esp8266':
            comp.add_comp_deps('kernel/vcall/espos')

        if aos_global_config.board == 'linuxhost' or aos_global_config.board == 'mk3060' \
                or aos_global_config.board == 'mk3239' or aos_global_config.board == 'mk3166' or aos_global_config.board == 'mk3165':
            comp.add_sources('mico/mico_rhino.c')
        comp.add_sources('aos/aos_rhino.c')
vcall_pre(component)
