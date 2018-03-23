src = Split('''
    mqtt_client.c
''')

MQTT_UTILS_PATH =  '../../../utility/iotx-utils'

if aos_global_config.board == 'linuxhost':
    PLATFORM_MQTT = 'linux'
    #component.add_component_dependencis('utility/iotx-utils/hal/linux')
else:    
    PLATFORM_MQTT = 'rhino'
    #component.add_component_dependencis('utility/iotx-utils/hal/rhino')


src.append( MQTT_UTILS_PATH+'/hal/'+PLATFORM_MQTT+'/HAL_OS_'+PLATFORM_MQTT+'.c' )    
src.append( MQTT_UTILS_PATH+'/hal/'+PLATFORM_MQTT+'/HAL_TCP_'+PLATFORM_MQTT+'.c' )    
src.append( MQTT_UTILS_PATH+'/mbedtls-hal/HAL_TLS_mbedtls.c' )

component = aos_component('mqtt', src)

dependencis = Split('''
    framework/connectivity/mqtt/MQTTPacket
    security/mbedtls
    framework/connectivity/mqtt
    utility/digest_algorithm 
    utility/iotx-utils/LITE-log
    utility/iotx-utils/LITE-utils
    utility/iotx-utils/misc 
    utility/iotx-utils/sdk-impl 
    utility/iotx-utils/guider 
    utility/iotx-utils/digest
''')
for i in dependencis:
    component.add_component_dependencis(i)


    
    

   

   
