src = Split('''
    netmgr.c
''')
component = aos_component('netmgr', src)

dependencis = Split('''
    kernel/fs/kv
    kernel/yloop
    kernel/hal
''')
for i in dependencis:
    component.add_comp_deps(i)

include_tmp = Split('''
    include
    ../middleware/alink/protocol/os/platform/
''')
for i in include_tmp:
    component.add_global_includes(i)

macro_tmp = Split('''
    AOS_COMP_NETMGR
''')
for i in macro_tmp:
    component.add_global_macros(i)




