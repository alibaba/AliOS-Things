src     = Split('''
        kvmgr.c
''')

component = aos_component('kv', src)

component.add_comp_deps('utility/log')

component.add_global_includes('include')

component.add_global_macros('AOS_KV')
