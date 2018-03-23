src = Split('''
    fota_test.c
''')

component = aos_component('fota_test', src)

component.add_component_dependencis('framework/fota')