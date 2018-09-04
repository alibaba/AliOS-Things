src     = Split('''
        board.c
        clock_config.c
        pin_mux.c
''')

component = aos_board_component('board_lpcxpresso54102', 'lpc54102', src)
component.add_global_macros('CPU_LPC54102J512BD64_cm4', '__USE_CMSIS', '__MULTICORE_MASTER', '__NEWLIB__')
aos_global_config.arch = 'Cortex-M4'

linux_only_targets="helloworld alinkapp mqttapp tls networkapp wifihalapp hdlcapp.hdlcclient linkkitapp nano coapapp vflashdemo linkkit_gateway helloworld_nocli uDataapp linuxapp hdlcapp.hdlcserver linkkit_sched acapp netmgrapp"
