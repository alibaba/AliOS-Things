src = Split('''
    src/und.c
    src/und_packet.c
    src/und_manage.c
    src/und_sched.c
    src/und_adapter.c
    hal/und_wifi.c
    hal/und_ble.c
''')
component = aos_component('und', src)

dependencis = Split('''
    kernel/fs/kv
    kernel/yloop
    kernel/hal
''')
for i in dependencis:
    component.add_comp_deps(i)

include_tmp = Split('''
    include
    middleware/linkkit/sdk-c/include
    middleware/linkkit/sdk-c/src/infra/log
''')
for i in include_tmp:
    component.add_global_includes(i)

macro_tmp = Split('''
    AOS_UND
''')
for i in macro_tmp:
    component.add_global_macros(i)

