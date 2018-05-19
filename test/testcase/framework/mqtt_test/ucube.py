src = Split('''
    mqtt_test.c
''')

component = aos_component('mqtt_test', src)

component.add_comp_deps('framework/connectivity/mqtt')
