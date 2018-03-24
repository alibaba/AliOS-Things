src = []

component = aos_component('fota_socket', src)

STM32_NONSTD_SOCKET = aos_global_config.get_aos_global_config('STM32_NONSTD_SOCKET')

if STM32_NONSTD_SOCKET == 'true':
    component.add_component_dependencis('framework/fota/download/http/socket/stm32wifi')
else:
    component.add_component_dependencis('framework/fota/download/http/socket/stand')