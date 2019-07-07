src     = ['board.c']

component = aos_board_component('board_mk3060', 'moc108', src)
aos_global_config.add_ld_files('memory.ld.S')

build_types=""

linux_only_targets="athostapp blink coapapp helloworld http2app httpapp id2_app itls_app linkkit_gateway linkkitapp lwm2mapp meshapp modbus_demo mqttapp otaapp prov_app tls udata_demo.sensor_cloud_demo udata_demo.sensor_local_demo udata_demo.udata_cloud_demo udata_demo.udata_local_demo udataapp udevapp ulocation.baseapp yts"
