src     = ['aos/board_partition.c', 'aos/soc_init.c', 'Src/stm32l4xx_hal_msp.c']

component = aos_board_component('stm32l496g-discovery', 'stm32l4xx_cube', src)

if aos_global_config.compiler == "armcc":
    component.add_sources('startup_stm32l496xx_keil.s')
    component.add_prebuilt_objs('startup_stm32l496xx_keil.o', 'Src/stm32l4xx_hal_msp.o')
elif aos_global_config.compiler == "iar":
    component.add_sources('startup_stm32l496xx_iar.s')
else:
    component.add_sources('startup_stm32l496xx.s')
    aos_global_config.add_ld_files('STM32L496AGIx_FLASH.ld')

component.add_global_includes('hal', 'aos', 'Inc')
component.add_global_macros('STM32L496xx', 'SYSINFO_PRODUCT_MODEL=\\"ALI_AOS_496-discovery\\"', 'SYSINFO_DEVICE_NAME=\\"496-discovery\\"')

if aos_global_config.get('sal', 1) == 1:
    component.add_comp_deps('network/sal')
linux_only_targets="at_app blink coapapp debug_app helloworld http2app jsengine_app kernel_app linkkit_gateway linkkitapp lwm2m_app modbus_app mqttapp otaapp sal_app udata_demo.sensor_cloud_demo udata_demo.sensor_local_demo udata_demo.udata_cloud_demo udata_demo.udata_local_demo udataapp ulocation.baseapp ulog_app yloop_app yts"
windows_only_targets="helloworld|COMPILER=armcc helloworld|COMPILER=iar"
