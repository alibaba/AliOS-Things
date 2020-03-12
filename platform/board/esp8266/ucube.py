src     = Split('''
        board.c
''')

component = aos_board_component('board_esp8266-demo', 'esp8266', src)

platform_options="wifi=1"
linux_only_targets="blink_demo coap_demo debug_demo hal_demo helloworld_demo http2_demo http_demo kernel_demo.aos_api_demo kernel_demo.krhino_api_demo linkkit_demo linkkit_gateway_demo mqtt_demo ota_demo ulocation.base_demo ulog_demo vfs_demo yts_demo"
