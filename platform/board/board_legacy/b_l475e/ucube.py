src     = Split('''
        board.c
        osa_flash.c
''')


component = aos_board_component('board_b_l475e', 'stm32l475', src)

if aos_global_config.get('sal') == None:
    aos_global_config.set('sal',1)
if aos_global_config.get('sal') == 1:
    component.add_comp_deps('network/sal')
else:
    component.add_global_macros('CONFIG_NO_TCPIP')

component.add_global_includes('.')

component.add_global_macros('STDIO_UART=0')
component.add_global_macros('RHINO_CONFIG_TICK_TASK=0')
component.add_global_macros('STM32L475xx')

CONFIG_SYSINFO_PRODUCT_MODEL = 'ALI_AOS_B-L475E'
CONFIG_SYSINFO_DEVICE_NAME = 'B-L475E'
aos_global_config.set('CONFIG_SYSINFO_PRODUCT_MODEL',CONFIG_SYSINFO_PRODUCT_MODEL)
aos_global_config.set('CONFIG_SYSINFO_DEVICE_NAME',CONFIG_SYSINFO_DEVICE_NAME)

component.add_global_macros('SYSINFO_PRODUCT_MODEL=\\"'+CONFIG_SYSINFO_PRODUCT_MODEL+'\\"')
component.add_global_macros('SYSINFO_DEVICE_NAME=\\"'+CONFIG_SYSINFO_DEVICE_NAME+'\\"')
component.add_global_macros('RHINO_CONFIG_WORKQUEUE=0')


linux_only_targets="at_app blink coapapp helloworld http2app jsengine_app linkkit_gateway linkkitapp modbus_app mqttapp otaapp sal_app udata_demo.sensor_cloud_demo udata_demo.sensor_local_demo udata_demo.udata_cloud_demo udata_demo.udata_local_demo udataapp ulocation.baseapp ulog_app yloop_app yts"
windows_only_targets="helloworld|COMPILER=armcc helloworld|COMPILER=iar"
