src = Split('''
    chip_code.c
''')

component = aos_component('chip_code', src)
component.add_includes('.')
