src = Split('''
    hashtable_test.c
''')

component = aos_component('hashtable_test', src)

component.add_comp_deps('utility/hashtable')

component.add_cflags('-Wall')
component.add_cflags('-Werror')