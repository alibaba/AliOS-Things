src = Split('''
    digest_algorithm_test.c
''')

component = aos_component('digest_algorithm_test', src)

component.add_component_dependencis('utility/digest_algorithm')

component.add_cflags('-Wall')
component.add_cflags('-Werror')