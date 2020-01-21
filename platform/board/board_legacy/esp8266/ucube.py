src     = Split('''
        board.c
''')

component = aos_board_component('board_esp8266', 'esp8266', src)

platform_options="wifi=1"
linux_only_targets="blink coapapp helloworld http2app httpclient_app jsengine_app linkkit_gateway linkkitapp mqttapp otaapp  ulocation.baseapp yts"
