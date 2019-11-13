src = Split('''
    ble_test.c
''')

component = aos_component('ble_test', src)

component.add_comp_deps('network/bluetooth/bt_host')

component.add_cflags('-Wall')
component.add_cflags('-Werror')
