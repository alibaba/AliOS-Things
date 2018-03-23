src = Split('''
    ota_transport.c
''')

component = aos_component('fota_mqtt_transport', src)

include_tmp = Split('''
    ..
    ../..
    ../../../mqtt/sdk-impl
    ../../../mqtt/platform    
''')

for i in include_tmp:
    component.add_global_includes(i)
    