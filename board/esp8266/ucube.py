src     = Split('''
        board.c
''')

component = aos_board_component('board_esp8266', 'esp8266', src)

platform_options="wifi=1"
linux_only_targets="helloworld acapp linkkitapp mqttapp coapapp linkkit_gateway linkkit_sched linuxapp helloworld_nocli nano netmgrapp wifihalapp tls networkapp"
