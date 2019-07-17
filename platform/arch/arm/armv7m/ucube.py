src = []

component = aos_arch_component('armv7m', src)
component.add_sources('common/panic_c.c', 'common/port_c.c', 'common/backtrace.c')
component.add_global_includes('common/')

if aos_global_config.compiler == 'armcc':
    component.add_global_asflags('--cpreproc')
    component.add_sources('common/panic_armcc.S')
    if aos_global_config.arch == 'Cortex-M3':
        component.add_sources('armcc/m3/port_s.S')
    elif aos_global_config.arch == 'Cortex-M4':
        component.add_sources('armcc/m4/port_s.S')
    elif aos_global_config.arch == 'Cortex-M7':
        component.add_sources('armcc/m7/port_s.S')
elif aos_global_config.compiler == 'iar':
    component.add_sources('common/panic_iccarm.S')
    if aos_global_config.arch == 'Cortex-M3':
        component.add_sources('iccarm/m3/port_s.S')
    elif aos_global_config.arch == 'Cortex-M4':
        component.add_sources('iccarm/m4/port_s.S')
    elif aos_global_config.arch == 'Cortex-M7':
        component.add_sources('iccarm/m7/port_s.S')
else:
    component.add_sources('common/panic_gcc.S')
    if aos_global_config.arch == 'Cortex-M3':
        component.add_sources('gcc/m3/port_s.S')
    elif aos_global_config.arch == 'Cortex-M4':
        component.add_sources('gcc/m4/port_s.S')
    elif aos_global_config.arch == 'Cortex-M7':
        component.add_sources('gcc/m7/port_s.S')

