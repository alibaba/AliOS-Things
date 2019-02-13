src     = Split('''
        board.c
''')

component = aos_board_component('board_esp8266', 'esp8266', src)

platform_options="wifi=1"
linux_only_targets="blink coapapp helloworld http2app linkkit_gateway linkkitapp mqttapp otaapp tls udevapp yts"
