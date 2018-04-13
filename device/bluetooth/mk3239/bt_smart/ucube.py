src =Split(''' 
    mico_bt_management.c
    mico_bt_peripheral.c
    mico_bt_smartbridge_gatt.c
    mico_bt_smartbridge.c
    mico_bt_smartbridge_cfg.c
    internal/bt_peripheral_stack_interface.c
    internal/bt_smart_attribute.c
    internal/bt_smartbridge_att_cache_manager.c
    internal/bt_smartbridge_socket_manager.c
    internal/bt_smartbridge_helper.c
    internal/bt_smartbridge_stack_interface.c
    LinkListUtils.c
    StringUtils.c
''')
component =aos_component('bt_smart', src)


global_includes =Split(''' 
    include
''')
for i in global_includes:
    component.add_global_includes(i)


includes =Split(''' 
    internal
''')
for i in includes:
    component.add_includes(i)


