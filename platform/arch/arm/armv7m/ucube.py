if aos_global_config.compiler == 'armcc':
    src = Split('''
        armcc/m4/port_c.c
        armcc/m4/port_s.S
    ''')
    component = aos_component('armv7m', src)
    component.add_global_includes('armcc/m4/')
elif aos_global_config.compiler == 'iar':
    src = Split('''
        iccarm/m4/port_c.c
        iccarm/m4/port_s.S
    ''')
    component = aos_component('armv7m', src)
    component.add_global_includes('iccarm/m4/')
else:
    src = Split('''
        gcc/m4/port_c.c
        gcc/m4/port_s.S
    ''')
    component = aos_component('armv7m', src)
    component.add_global_includes('gcc/m4/')