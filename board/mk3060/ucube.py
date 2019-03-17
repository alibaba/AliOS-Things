src     = ['board.c']

component = aos_board_component('board_mk3060', 'moc108', src)
aos_global_config.add_ld_files('memory.ld.S')

build_types=""

linux_only_targets="athostapp blink coapapp helloworld http2app httpapp id2_app itls_app linkkit_gateway linkkitapp meshapp modbus_demo mqttapp otaapp prov_app tls udataapp udevapp ulocationapp yts"
