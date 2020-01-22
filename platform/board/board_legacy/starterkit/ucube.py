src =Split('''
    aos/board.c
    aos/board_cli.c
    aos/st7789.c
    aos/soc_init.c
    Src/stm32l4xx_hal_msp.c
''')
component =aos_board_component('starterkit', 'stm32l4xx_cube', src)

if aos_global_config.compiler == "armcc":
    component.add_sources('startup_stm32l433xx_keil.s')
elif aos_global_config.compiler == "iar":
    component.add_sources('startup_stm32l433xx_iar.s')
else:
    component.add_sources('startup_stm32l433xx.s')
    aos_global_config.add_ld_files('STM32L433RCTxP_FLASH.ld')

aos_global_config.set('MODULE', '1062')
aos_global_config.set('HOST_ARCH', 'Cortex-M4')
aos_global_config.set('HOST_MCU_FAMILY', 'stm32l4xx_cube')
aos_global_config.set('SUPPORT_MBINS', 'no')
aos_global_config.set('HOST_MCU_NAME', 'STM32L433CCTx')

dependencis =Split('''
    drivers/sensor
''')
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split('''
    hal
    aos
    Inc
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split('''
    STM32L433xx
    STDIO_UART=0
    AOS_SENSOR_ACC_MIR3_DA217
    AOS_SENSOR_ALS_LITEON_LTR553
    AOS_SENSOR_PS_LITEON_LTR553
    AOS_SENSOR_ACC_SUPPORT_STEP
    AOS_TEST_MODE
''')
for i in global_macros:
    component.add_global_macros(i)

if aos_global_config.get('sal') == None:
    aos_global_config.set('sal',1)

if aos_global_config.get('no_') == None:
    aos_global_config.set('no_',1)

if aos_global_config.get('sal') == 1:
    component.add_comp_deps('network/sal')
    if aos_global_config.get('module') == None:
        aos_global_config.set('module','wifi.bk7231')
    else:
        aos_global_config.add_global_macros('CONFIG_NO_TCPIP')

aos_global_config.set('CONFIG_SYSINFO_PRODUCT_MODEL', 'ALI_AOS_starterkit')
aos_global_config.set('CONFIG_SYSINFO_DEVICE_NAME','starterkit')


component.add_global_macros('SYSINFO_PRODUCT_MODEL=\\"'+'ALI_AOS_starterkit'+'\\"')
component.add_global_macros('SYSINFO_DEVICE_NAME=\\"'+'starterkit'+'\\"')

component.set_enable_vfp()

linux_only_targets="at_app blink coapapp debug_app helloworld http2app jsengine_app kernel_app modbus_app mqttapp otaapp sal_app sk.sk_gui udata_demo.sensor_cloud_demo udata_demo.sensor_local_demo udata_demo.udata_cloud_demo udata_demo.udata_local_demo udataapp ulocation.baseapp ulog_app yloop_app yts"
windows_only_targets="helloworld|COMPILER=armcc helloworld|COMPILER=iar"
