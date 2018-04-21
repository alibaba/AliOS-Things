src =Split(''' 
    adv_helper.c
    init_helper.c
    gatt_attr_helper.c
''')
component =aos_component('ble_app_helper_impl', src)

dependencis =Split(''' 
    device/bluetooth/mk3239/bt_smart
    framework/bluetooth/ble_app_framework
    device/bluetooth/mk3239/low_energy
''')
for i in dependencis:
    component.add_comp_deps(i)





