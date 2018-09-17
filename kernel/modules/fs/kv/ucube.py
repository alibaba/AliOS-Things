src     = Split('''
''')

component = aos_component('kv_aos', src)

component.add_comp_deps('kernel/rhino/fs/kv')

