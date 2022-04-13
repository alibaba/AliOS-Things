src     = Split('''
        cpu_pwr_hal_lib.c
        cpu_pwr_lib.c
        pwrmgmt_debug.c
        cpu_tickless.c
        pwrmgmt.c
''')
component = aos_component('pwrmgmt', src)

component.add_global_includes('include')
component.add_global_macros('AOS_COMP_PWRMGMT')
