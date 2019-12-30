src     = Split('''
        cli.c
        cli_dumpsys.c
        cli_adapt.c
        cli_cpuusage.c
        cli_default_command.c
''')
component = aos_component('cli', src)

component.add_comp_deps('kernel/hal')

component.add_global_macros('AOS_COMP_CLI')


component.add_global_includes('include')
