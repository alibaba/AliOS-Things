src = []

component = aos_component('osal', src)
component.add_global_includes('mico/include', 'include')

component.add_comp_deps("middleware/alink/cloud")
if aos_global_config.arch == 'ARM968E-S':
    component.add_cflags('-marm')

@pre_config('osal')
def osal_pre(comp):
    osal = aos_global_config.get('osal', 'rhino')
    if osal == 'freertos':
        comp.add_global_macros('OSAL_FREERTOS')
        comp.add_sources('aos/freertos.c')
    elif osal == 'posix':
        comp.add_global_macros('OSAL_POSIX')
        comp.add_sources('aos/posix.c')
    elif osal == 'winmsvs':
        comp.add_global_macros('OSAL_MSVS')
        comp.add_sources('aos/winnt.c')
    else:
        comp.add_global_macros('OSAL_RHINO')
        comp.add_comp_deps('kernel/rhino')

        if aos_global_config.mcu_family == 'esp32' or aos_global_config.mcu_family == 'esp8266':
            comp.add_comp_deps('osal/espos')

        if aos_global_config.board == 'linuxhost' or aos_global_config.board == 'mk3060' \
                or aos_global_config.board == 'mk3239' or aos_global_config.board == 'mk3166' or aos_global_config.board == 'mk3165':
            comp.add_sources('mico/mico_rhino.c')
        comp.add_sources('aos/rhino.c')
osal_pre(component)
