src = Split('''
    simple_mqtt.c
''')

component = aos_component('simple_mqtt', src)
component.add_component_dependencis(
    'kernel/yloop',
    'tools/cli',
    'framework/connectivity/mqtt',
    'utility/cjson',
    'framework/fota',
    'framework/netmgr',
    'framework/common'
)
