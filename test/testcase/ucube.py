src   = Split('''
    yts_main.c
''')

component = aos_component('testcase', src)
component.add_global_includes('include')
component.add_comp_deps('test/yunit')
if aos_global_config.compiler == 'gcc':
    component.add_cflags( "-Wall" )
    component.add_cflags( "-Werror")


