src     = Split('''
        board.c
        osa_flash.c
''')

component = aos_component('board_b_l475e', src)
component.add_component_dependencis('platform/mcu/stm32l475')
component.add_component_dependencis('device/sal')

aos_global_config.set_aos_global_config('module','wifi.mk3060')

component.add_global_includes('.')

component.add_global_macro('STDIO_UART=0')
component.add_global_macro('RHINO_CONFIG_TICK_TASK=0')
component.add_global_macro('STM32L475xx')

CONFIG_SYSINFO_PRODUCT_MODEL = 'ALI_AOS_B-L475E'
CONFIG_SYSINFO_DEVICE_NAME = 'B-L475E'
aos_global_config.set_aos_global_config('CONFIG_SYSINFO_PRODUCT_MODEL',CONFIG_SYSINFO_PRODUCT_MODEL)
aos_global_config.set_aos_global_config('CONFIG_SYSINFO_DEVICE_NAME',CONFIG_SYSINFO_DEVICE_NAME)

component.add_global_macro('SYSINFO_PRODUCT_MODEL=\\"'+CONFIG_SYSINFO_PRODUCT_MODEL+'\\"')
component.add_global_macro('SYSINFO_DEVICE_NAME=\\"'+CONFIG_SYSINFO_DEVICE_NAME+'\\"')
component.add_global_macro('RHINO_CONFIG_WORKQUEUE=0')

