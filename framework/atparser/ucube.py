src     = Split('''
        atparser.c
''')

component = aos_component('atparser', src)

component.add_global_includes('./')

component.add_global_macro('AOS_ATCMD')