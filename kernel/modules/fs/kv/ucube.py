src     = Split('''
        kvmgr.c
''')

component = aos_component('kv', src)

component.add_component_dependencis('utility/log')

component.add_global_includes('include')

component.add_global_macro('AOS_KV')
