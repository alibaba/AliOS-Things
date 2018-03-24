src     = Split('''
        cpu_impl.c
        swap.S
''')
aos_component('linux', src)
