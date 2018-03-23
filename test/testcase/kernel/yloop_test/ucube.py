src = Split('''
    yloop_test.c
''')

component = aos_component('yloop_test', src)

component.add_component_dependencis('kernel/yloop')

component.add_cflags('-Wall')
component.add_cflags('-Werror')