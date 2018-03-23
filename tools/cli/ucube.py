src     = Split('''
        cli.c 
        dumpsys.c
''')
component = aos_component('cli', src)

component.add_component_dependencis('kernel/hal')

component.add_global_macro('HAVE_NOT_ADVANCED_FORMATE')
component.add_global_macro('CONFIG_AOS_CLI')


component.add_global_includes('include')
