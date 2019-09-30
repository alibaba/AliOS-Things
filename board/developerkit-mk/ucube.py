src =Split('''
    aos/board.c
    aos/board_cli.c
    aos/st7789.c
    aos/soc_init.c
    pwrmgmt_hal/board_cpu_pwr.c
    pwrmgmt_hal/board_cpu_pwr_rtc.c
    pwrmgmt_hal/board_cpu_pwr_systick.c
    pwrmgmt_hal/board_cpu_pwr_timer.c
    mbmaster_hal/port_serial.c
    Src/adc.c
    Src/crc.c
    Src/dcmi.c
    Src/dma.c
    Src/i2c.c
    Src/irtim.c
    Src/main.c
    Src/sai.c
    Src/sdmmc.c
    Src/spi.c
    Src/stm32l4xx_hal_msp.c
    Src/tim.c
    Src/usart.c
    Src/usb_otg.c
''')
component =aos_board_component('developerkit', 'stm32l4xx_cube', src)

if aos_global_config.compiler == "armcc":
    component.add_sources('startup_stm32l496xx_keil.s')
elif aos_global_config.compiler == "iar":
    component.add_sources('startup_stm32l496xx_iar.s')
else:
    component.add_sources('startup_stm32l496xx.s')
    aos_global_config.add_ld_files('STM32L496VGTx_FLASH.ld')

aos_global_config.set('MODULE', '1062')
aos_global_config.set('HOST_ARCH', 'Cortex-M4')
aos_global_config.set('HOST_MCU_FAMILY', 'stm32l4xx_cube')
aos_global_config.set('SUPPORT_MBINS', 'no')
aos_global_config.set('HOST_MCU_NAME', 'STM32L496VGTx')

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
    STM32L496xx
    STDIO_UART=0
    AOS_SENSOR_BARO_BOSCH_BMP280
    AOS_SENSOR_ALS_LITEON_LTR553
    AOS_SENSOR_PS_LITEON_LTR553
    AOS_SENSOR_HUMI_SENSIRION_SHTC1
    AOS_SENSOR_TEMP_SENSIRION_SHTC1
    AOS_SENSOR_ACC_ST_LSM6DSL
    AOS_SENSOR_MAG_MEMSIC_MMC3680KJ
    AOS_SENSOR_GYRO_ST_LSM6DSL
''')
for i in global_macros:
    component.add_global_macros(i)

if aos_global_config.get('sal') == None:
    aos_global_config.set('sal',1)

if aos_global_config.get('sal') == 1:
    component.add_comp_deps('network/sal')
    if aos_global_config.get('module') == None:
        aos_global_config.set('module','wifi.bk7231')
    else:
        aos_global_config.add_global_macros('CONFIG_NO_TCPIP')

print('>>> Include Module %s'%aos_global_config.get('module'))

aos_global_config.set('CONFIG_SYSINFO_PRODUCT_MODEL', 'ALI_AOS_developerkit')
aos_global_config.set('CONFIG_SYSINFO_DEVICE_NAME','developerkit')


component.add_global_macros('SYSINFO_PRODUCT_MODEL=\\"'+'ALI_AOS_developerkit'+'\\"')
component.add_global_macros('SYSINFO_DEVICE_NAME=\\"'+'developerkit'+'\\"')

component.set_enable_vfp()

linux_only_targets="blink coapapp dk.dk_audio dk.dk_camera dk.dk_gui dk.dk_qr helloworld http2app linkkit_gateway linkkitapp modbus_demo mqttapp otaapp tls udata_demo.sensor_cloud_demo udata_demo.sensor_local_demo udata_demo.udata_cloud_demo udata_demo.udata_local_demo udataapp ulocation.baseapp yts"
windows_only_targets="helloworld|COMPILER=armcc helloworld|COMPILER=iar linkkitapp|COMPILER=armcc"
