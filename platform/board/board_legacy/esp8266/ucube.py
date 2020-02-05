src     = Split('''
        board.c
''')

component = aos_board_component('board_esp8266', 'esp8266', src)

platform_options="wifi=1"
linux_only_targets="blink coapapp debug_app helloworld http2app httpapp httpclient_app jsengine_app linkkit_gateway linkkitapp mqttapp otaapp uart udata_demo.sensor_cloud_demo udata_demo.sensor_local_demo ulocation.baseapp ulog_app websoc_app yloop_app yts"
