src = []
if aos_global_config.compiler == 'armcc':
    component = aos_arch_component('armv7m', src)
    component.add_global_asflags('--cpreproc')
    component.add_sources('panic/panic_c.c', 'panic/panic_armcc.S')
    if aos_global_config.arch == 'Cortex-M3':
        component.add_sources('armcc/m3/port_c.c', 'armcc/m3/port_s.S')
        component.add_global_includes('armcc/m3')
    elif aos_global_config.arch == 'Cortex-M4':
        component.add_sources('armcc/m4/port_c.c', 'armcc/m4/port_s.S')
        component.add_global_includes('armcc/m4')
    elif aos_global_config.arch == 'Cortex-M7':
        component.add_sources('armcc/m7/port_c.c', 'armcc/m7/port_s.S')
        component.add_global_includes('armcc/m7')
elif aos_global_config.compiler == 'iar':
    component = aos_arch_component('armv7m', src)
    component.add_sources('panic/panic_c.c', 'panic/panic_iccarm.S')
    if aos_global_config.arch == 'Cortex-M3':
        component.add_sources('iccarm/m3/port_c.c', 'iccarm/m3/port_s.S')
        component.add_global_includes('iccarm/m3')
    elif aos_global_config.arch == 'Cortex-M4':
        component.add_sources('iccarm/m4/port_c.c', 'iccarm/m4/port_s.S')
        component.add_global_includes('iccarm/m4')
    elif aos_global_config.arch == 'Cortex-M7':
        component.add_sources('iccarm/m7/port_c.c', 'iccarm/m7/port_s.S')
        component.add_global_includes('iccarm/m7')
else:
    component = aos_arch_component('armv7m', src)
    component.add_sources('panic/panic_c.c', 'panic/panic_gcc.S')
    if aos_global_config.arch == 'Cortex-M3':
        component.add_sources('gcc/m3/port_c.c', 'gcc/m3/port_s.S')
        component.add_global_includes('gcc/m3')
    elif aos_global_config.arch == 'Cortex-M4':
        component.add_sources('gcc/m4/port_c.c', 'gcc/m4/port_s.S')
        component.add_global_includes('gcc/m4')
    elif aos_global_config.arch == 'Cortex-M7':
        component.add_sources('gcc/m7/port_c.c', 'gcc/m7/port_s.S')
        component.add_global_includes('gcc/m7')

