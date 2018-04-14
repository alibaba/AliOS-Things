src =Split(''' 
    ble_access_core.c
    ble_access_core_i.c
''')
component =aos_component('Lib_Ble_Access_Core_Framework', src)

global_includes =Split(''' 
    .
''')
for i in global_includes:
    component.add_global_includes(i)




