src = []
include_tmp = []

if aos_global_config.compiler == 'armcc':
    src = Split('''
        compilers/armlibc/armcc_libc.c
    ''')
    include_tmp = Split('''
        compilers/armlibc
    ''')       
elif aos_global_config.compiler == 'iar':
    src = Split('''
        compilers/iar/iar_libc.c
    ''')
    include_tmp = Split('''
        compilers/iar
    ''')  
elif aos_global_config.board != 'linuxhost':
    src = Split('''
        newlib_stub.c
    ''')

component = aos_component('newlib_stub', src)
for i in include_tmp:
    component.add_global_includes(i)