src     = Split('''
        kv_core.c
        kv_adapt.c
        kv_cli.c
''')

component = aos_component('kv', src)

component.add_global_includes('include')

component.add_global_macros('AOS_COMP_KV')
