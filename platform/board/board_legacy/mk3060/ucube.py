src     = ['board.c']

component = aos_board_component('board_mk3060', 'moc108', src)
aos_global_config.add_ld_files('memory.ld.S')

build_types=""

linux_only_targets="appdemo blink coapapp debug_app helloworld http2app httpapp httpclient_app httpdns_app id2_app itls_app jsengine_app kernel_app linkkit_gateway linkkitapp lwm2m_app meshapp modbus_app mqttapp otaapp prov_app udata_demo.sensor_cloud_demo udata_demo.sensor_local_demo udata_demo.udata_cloud_demo udata_demo.udata_local_demo udataapp ulocation.baseapp ulog_app websoc_app yloop_app yts"
