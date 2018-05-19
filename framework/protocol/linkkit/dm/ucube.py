src =Split(''' 
    src/dm_cm_impl.c
    src/dm_cm_msg_info.c
    src/dm_impl.c
    src/dm_thing.c
    src/dm_thing_manager.c
    src/dm_logger.c
    src/dm_slist.c
''')
component =aos_component('dm', src)

dependencis =Split(''' 
    framework/common
''')
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split(''' 
    ./include
    ./include/interface
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    USING_UTILS_JSON
    LITE_THING_MODEL
    NDEBUG
    LOCAL_CONN_ENABLE
''')
for i in global_macros:
    component.add_global_macros(i)

includes =Split(''' 
    ../../protocol/alink-ilop/sdk-encap
    ../../protocol/alink-ilop/iotkit-system
    ../../protocol/cm/
''')
for i in includes:
    component.add_includes(i)
