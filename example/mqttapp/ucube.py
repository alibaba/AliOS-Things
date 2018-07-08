src = Split('''
    mqtt-example.c
''')
component = aos_component('mqttapp', src)

if aos_global_config.get('sal') == 1:
	component.add_global_macros('MQTT_PRESS_TEST')
	
if aos_global_config.get('no_tls') == 1:
    component.add_global_macros('IOTX_WITHOUT_TLS')
    component.add_global_macros('MQTT_DIRECT')

macro_tmp = Split('''
    MQTT_TEST
    ALIOT_DEBUG 
    IOTX_DEBUG
    USE_LPTHREAD
''')
for i in macro_tmp:
    component.add_global_macros(i)

dependencis = Split('''
    tools/cli 
    framework/connectivity/mqtt
    utility/cjson 
    framework/fota 
    framework/netmgr 
    framework/common
''')
for i in dependencis:
    component.add_comp_deps(i)
