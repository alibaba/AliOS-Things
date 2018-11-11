src     = ['board.c', 'mico_spi.c']

component = aos_board_component('board_mk3165', 'stm32f4xx', src)
component.add_prebuilt_libs('MiCO.3165.GCC.a')

global_macros = Split('''
    HSE_VALUE=26000000
    STDIO_UART=0
    RHINO_CONFIG_TICK_TASK=0
    RHINO_CONFIG_WORKQUEUE=0
''')
component.add_global_macros(*global_macros)

aos_global_config.add_ld_files('memory.ld')
aos_global_config.set('HOST_MCU_VARIANT', 'STM32F411')
aos_global_config.set('BUS', 'SDIO')

linux_only_targets="wifimonitor linkkitapp udataapp kernel_test nano otaapp yts networkapp vflashdemo netmgrapp linkkit_gateway wifihalapp hdlcapp.hdlcserver cryptotest tls mqttapp helloworld_nocli acapp coapapp helloworld alinkapp http2app"
