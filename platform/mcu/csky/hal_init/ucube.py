src     = Split('''
        hal_init.c
''')

component = aos_component('hal_init', src)
