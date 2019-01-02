src = Split('''
        port.c
        panic.c
        backtrace.c
        xtensa_panic.S
        xtensa_vectors.S
''')


component = aos_component('xtensa', src)
component.add_global_includes('./include', './include/frxt')
