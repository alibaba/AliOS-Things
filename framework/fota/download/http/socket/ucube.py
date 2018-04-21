src = []

component = aos_component('fota_socket', src)

STM32_NONSTD_SOCKET = aos_global_config.get('STM32_NONSTD_SOCKET')

if STM32_NONSTD_SOCKET == 'true':
    component.add_comp_deps('framework/fota/download/http/socket/stm32wifi')
else:
    component.add_comp_deps('framework/fota/download/http/socket/stand')