src = Split('''
    osal_test.c
''')

component = aos_component('osal_test', src)

component.add_comp_deps('osal')

component.add_cflags('-Wall')
component.add_cflags('-Werror')
