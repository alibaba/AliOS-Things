src     = ['aos/board_partition.c', 'aos/soc_init.c', 'Src/stm32l4xx_hal_msp.c']

component = aos_board_component('stm32l476rg-nucleo', 'stm32l4xx_cube', src)

if aos_global_config.compiler == "armcc":
    component.add_sources('startup_stm32l476xx_keil.s')
    component.add_prebuilt_objs('startup_stm32l433xx_keil.o', 'Src/stm32l4xx_hal_msp.o')
elif aos_global_config.compiler == "iar":
    component.add_sources('startup_stm32l476xx_iar.s')
else:
    component.add_sources('startup_stm32l476xx.s')
    aos_global_config.add_ld_files('STM32L476RGTx_FLASH.ld')

component.add_global_includes('hal', 'aos', 'Inc')
component.add_global_macros('STM32L476xx', 'SYSINFO_PRODUCT_MODEL=\\"ALI_AOS_476-nucleo\\"', 'SYSINFO_DEVICE_NAME=\\"476-nucleo\\"')

if aos_global_config.get('sal', 1) == 1:
    component.add_comp_deps('network/sal')
    module = aos_global_config.get('module', 'wifi.mk3060')
    if module == 'wifi.mk3060':
        aos_global_config.set('module', 'wifi.mk3060')

component.set_enable_vfp()

linux_only_targets="blink coapapp helloworld http2app jsengine_app linkkit_gateway linkkitapp modbus_demo mqttapp otaapp tls udata_demo.sensor_cloud_demo udata_demo.sensor_local_demo udata_demo.udata_cloud_demo udata_demo.udata_local_demo udataapp ulocation.baseapp yts"
windows_only_targets="blink|COMPILER=armcc blink|COMPILER=iar helloworld|COMPILER=armcc helloworld|COMPILER=iar linkkitapp|COMPILER=armcc"
