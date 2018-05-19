src = []
if aos_global_config.compiler == 'armcc':
    component = aos_component('armv7m', src)
    if aos_global_config.arch == 'Cortex-M4':
        component.add_sources('armcc/m4/port_c.c', 'armcc/m4/port_s.S')
        component.add_global_includes('armcc/m4')
elif aos_global_config.compiler == 'iar':
    component = aos_component('armv7m', src)
    if aos_global_config.arch == 'Cortex-M4':
        component.add_sources('iccarm/m4/port_c.c', 'iccarm/m4/port_s.S')
        component.add_global_includes('iccarm/m4')
else:
    component = aos_component('armv7m', src)
    if aos_global_config.arch == 'Cortex-M4':
        component.add_sources('gcc/m4/port_c.c', 'gcc/m4/port_s.S')
        component.add_global_includes('gcc/m4')
    elif aos_global_config.arch == 'Cortex-M7':
        component.add_sources('gcc/m7/port_c.c', 'gcc/m7/port_s.S')
        component.add_global_includes('gcc/m7')
