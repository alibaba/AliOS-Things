src     = Split('''
        functions/mbfunccoils.c
        functions/mbfuncholding.c
        port/port_event.c
        port/port_other.c
        port/port_timer.c
        rtu/mbcrc.c
        rtu/mbrtu.c
        mb.c
''')
component = aos_component('mbmaster', src)

component.add_global_includes('include')
component.add_global_macros('AOS_MBMASTER')
