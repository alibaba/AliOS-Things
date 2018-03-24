src   = Split('''
    yts_main.c
''')

component = aos_component('testcase', src)
component.add_global_includes('include')
component.add_component_dependencis('test/yunit')
component.add_cflags( "-Wall" )
component.add_cflags( "-Werror")


