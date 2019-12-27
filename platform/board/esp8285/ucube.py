src     = Split('''
        board.c
''')

component = aos_board_component('board_esp8266', 'esp8266', src)


linux_only_targets="blink coapapp helloworld http2app httpapp jsengine_app linkkit_gateway linkkitapp mqttapp otaapp  ulocation.baseapp yts"
