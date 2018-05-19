src = Split('''
   basic_test.c 
''')

component = aos_component('basic_test', src)

component.add_cflags('-Wall')
component.add_cflags('-Werror')
