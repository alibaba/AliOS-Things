src = Split('''
    rbtree_test.c
''')

component = aos_component('rbtree_test', src)

component.add_comp_deps('kernel/lib/rbtree')

component.add_cflags('-Wall')
component.add_cflags('-Werror')
