src = Split('''
    rhino_test.c
''')

component = aos_component('rhino_test', src)

component.add_comp_deps('kernel/rhino')

if aos_global_config.board == 'linuxhost':
    component.add_sources('arch/linux/port_test.c')

component.add_cflags('-Wall')
component.add_cflags('-Werror')