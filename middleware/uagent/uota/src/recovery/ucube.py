src = Split('''
    rec_libc.c
    rec_main.c
''')

component = aos_component('recovery', src)

component.add_global_includes('.')
