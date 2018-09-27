src     = Split('''
        at_mqtt_client.c
''')

component = aos_component('atmqttclient', src)

component.add_global_includes('./')

if aos_global_config.get('atmqttica') == 1:
    component.add_sources('at_mqtt_ica_client.c')



