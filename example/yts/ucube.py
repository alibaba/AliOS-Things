src     = Split('''
    main.c
''')

component = aos_component('yts', src)

#  testcase rhino.test log vfs yloop hal
component.add_component_dependencis('test/testcase')
component.add_component_dependencis('kernel/rhino/test')
component.add_component_dependencis('utility/log')
component.add_component_dependencis('kernel/vfs')
component.add_component_dependencis('kernel/yloop')
component.add_component_dependencis('kernel/hal')

if aos_global_config.board == 'linuxhost':
    component.add_component_dependencis('kernel/protocols/net')
    component.add_component_dependencis('kernel/protocols/mesh')
    component.add_component_dependencis('tools/dda')
    component.add_component_dependencis('framework/netmgr')
    component.add_component_dependencis('kernel/modules/fs/fatfs')
    component.add_component_dependencis('framework/common')
    component.add_global_macro('CONFIG_AOS_MESHYTS')
    component.add_global_macro('DEBUG')
    component.add_global_macro('YTS_LINUX')
else:
    component.add_component_dependencis('tools/cli')


