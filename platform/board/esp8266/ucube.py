src     = Split('''
        board.c
''')

component = aos_board_component('board_esp8266-demo', 'esp8266', src)

platform_options="wifi=1"
linux_only_targets="blink_demo hal_demo helloworld_demo kernel_demo.krhino_api_demo kernel_demo.aos_api_demo linkkit_demo linkkit_gateway_demo mqtt_demo ota_demo yts_demo"
