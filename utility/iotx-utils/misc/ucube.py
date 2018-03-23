src = Split('''
    utils_httpc.c
    utils_epoch_time.c
    utils_list.c
    utils_net.c
    utils_timer.c
''')

component = aos_component('libmisc', src)

component.add_macro('DEBUG')
component.add_includes('.', '../sdk-impl', '../LITE-log', '../LITE-utils')

component.add_component_dependencis('utility/iotx-utils/LITE-log')


if aos_global_config.board == 'linuxhost':
    PLATFORM_MISC = 'linux'
else:
    PLATFORM_MISC = 'rhino'

comp_names = [comp.name for comp in aos_global_config.components]
if 'coap' in  comp_names:
    component.add_sources( '../hal/'+PLATFORM_MISC+'/HAL_TCP_'+PLATFORM_MISC+'.c')
    
    CONFIG_COAP_DTLS_SUPPORT=aos_global_config.get_aos_global_config(' CONFIG_COAP_DTLS_SUPPORT')
    if CONFIG_COAP_DTLS_SUPPORT != 'y':
        component.add_sources( '../hal/'+PLATFORM_MISC+'/HAL_OS_'+PLATFORM_MISC+'.c')
        component.add_component_dependencis('utility/iotx-utils/mbedtls-hal')

    






    