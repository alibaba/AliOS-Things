src     = Split('''
        gcc/port_c.c
        gcc/port_s.S
        common/panic_c.c
        common/panic_gcc.S
''')

component = aos_component('armv5', src)
component.add_global_includes('common')

local_cflags = Split('''
        -marm
        -mthumb-interwork
''')
for cflags in local_cflags:
    component.add_cflags(cflags)
