src = Split('''
        port.c
        xtensa_panic.S
        xtensa_vectors.S
''')


component = aos_component('xtensa', src)
component.add_global_includes('./include', './include/frxt')
