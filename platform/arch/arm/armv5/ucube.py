src     = Split('''
        gcc/port_c.c
        gcc/port_s.S
''')

component = aos_component('armv5', src)
component.add_global_includes('gcc')

local_cflags = Split('''
        -marm
        -mthumb-interwork
''')
for cflags in local_cflags:
    component.add_cflags(cflags)
