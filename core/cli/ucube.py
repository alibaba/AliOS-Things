src     = Split('''
        cli.c 
        dumpsys.c
''')
component = aos_component('cli', src)

component.add_comp_deps('kernel/hal')

component.add_global_macros('AOS_COMP_CLI')


component.add_global_includes('include')
