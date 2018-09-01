src = ['board_init.c', 'net/ethernetif.c']

component = aos_board_component('board_cb2201', 'csky', src)

component.add_global_includes('include')
component.add_global_macros('STDIO_UART=0')
component.add_global_macros('MBEDTLS_AES_ROM_TABLES=1')

component.set_global_arch('ck802')
component.add_global_cflags('-std=gnu99')

aos_global_config.add_ld_files('gcc_csky.ld')

linux_only_targets="helloworld helloworld_nocli hdlcapp.hdlcclient networkapp wifihalapp uDataapp hdlcapp.hdlcserver itls_app nano mqttapp netmgrapp tls coapapp id2_app"
