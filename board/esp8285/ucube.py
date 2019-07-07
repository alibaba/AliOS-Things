src     = Split('''
        board.c
''')

component = aos_board_component('board_esp8266', 'esp8266', src)


linux_only_targets="blink coapapp helloworld http2app httpapp linkkit_gateway linkkitapp mqttapp otaapp tls udevapp ulocation.baseapp yts"
