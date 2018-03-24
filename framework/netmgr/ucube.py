src = Split('''
    netmgr.c
''')
component = aos_component('netmgr', src)

dependencis = Split('''
    kernel/modules/fs/kv
    kernel/yloop
    kernel/hal
''')
for i in dependencis:
    component.add_component_dependencis(i)
    
include_tmp = Split('''
    include 
    ../protocol/alink/os/platform/
''')
for i in include_tmp:
    component.add_global_includes(i)
    
macro_tmp = Split('''
    AOS_NETMGR
''')
for i in macro_tmp:
    component.add_global_macro(i)



    