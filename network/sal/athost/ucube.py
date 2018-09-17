src = Split('''
        ./athost.c
        ./util/athost_io.c
        ./util/at_util.c
        ./atcmd/at_uart.c
        ./atcmd/at_tcp_udp.c
        ./atcmd/at_wifi_mgmt.c
        ./atcmd/at_uart.c
        ./atcmd/at_wifi_ywss.c
''')

component = aos_component('athost', src)

global_includes = Split(''' 
                    ./include
                    ./include/internal
''')

for i in global_includes:
    component.add_global_includes(i)

coap = aos_global_config.get('coap', 1)

component.add_comp_deps('middleware/linkkit/sdk')
component.add_comp_deps('middleware/linkkit/hal')
component.add_comp_deps('network/netmgr')

if coap == 1:
	component.add_global_macros('COAP_DTLS_SUPPORT')
	component.add_global_macros('COAP_ONLINE')
	component.add_global_macros('COAP_AT')
    component.add_sources('./atcmd/at_coap.c')
else:
    component.add_global_macros('MQTT_TEST')
    component.add_sources('./atcmd/at_mqtt.c')


