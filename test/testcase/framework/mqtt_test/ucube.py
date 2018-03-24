src = Split('''
    mqtt_test.c
''')

component = aos_component('mqtt_test', src)

component.add_component_dependencis('framework/connectivity/mqtt')
