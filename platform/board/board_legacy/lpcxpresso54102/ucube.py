src     = Split('''
        board.c
        clock_config.c
        pin_mux.c
''')

component = aos_board_component('board_lpcxpresso54102', 'lpc54102', src)
component.add_global_macros('CPU_LPC54102J512BD64_cm4', '__USE_CMSIS', '__MULTICORE_MASTER', '__NEWLIB__')
aos_global_config.arch = 'Cortex-M4'

linux_only_targets="at_app coapapp debug_app helloworld http2app jsengine_app kernel_app linkkit_gateway linkkitapp lwm2m_app modbus_app mqttapp otaapp sal_app udata_demo.sensor_cloud_demo udata_demo.sensor_local_demo udataapp ulocation.baseapp ulog_app yloop_app yts"
