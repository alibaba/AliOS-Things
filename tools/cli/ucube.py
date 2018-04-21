src     = Split('''
        cli.c 
        dumpsys.c
''')
component = aos_component('cli', src)

component.add_comp_deps('kernel/hal')

component.add_global_macros('HAVE_NOT_ADVANCED_FORMATE')
component.add_global_macros('CONFIG_AOS_CLI')


component.add_global_includes('include')
