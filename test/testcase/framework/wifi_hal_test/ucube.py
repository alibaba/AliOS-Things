src = Split('''
    wifi_hal_test.c
''')

component = aos_component('wifi_hal_test', src)