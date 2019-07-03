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

linux_only_targets="coapapp helloworld http2app httpapp linkkit_gateway linkkitapp lwm2mapp modbus_demo mqttapp otaapp tls udata_demo.sensor_cloud_demo udata_demo.sensor_local_demo udata_demo.udata_cloud_demo udata_demo.udata_local_demo udataapp udevapp ulocationapp yts"
