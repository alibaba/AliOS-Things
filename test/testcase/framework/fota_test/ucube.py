src = Split('''
    fota_test.c
''')

component = aos_component('fota_test', src)

component.add_comp_deps('framework/fota')