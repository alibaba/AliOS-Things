src     = Split('''
    main.c
''')

component = aos_component('yts', src)

#  testcase rhino.test log vfs yloop hal
component.add_comp_deps('test/testcase')
component.add_comp_deps('kernel/rhino/test')
component.add_comp_deps('utility/log')
component.add_comp_deps('kernel/vfs')
component.add_comp_deps('kernel/yloop')
component.add_comp_deps('kernel/hal')

if aos_global_config.board == 'linuxhost':
    component.add_comp_deps('network/lwip')
    component.add_comp_deps('network/netmgr')
    component.add_comp_deps('3rdparty/experimental/fs/fatfs')
    component.add_global_macros('CONFIG_AOS_MESHYTS')
    component.add_global_macros('DEBUG')
    component.add_global_macros('YTS_LINUX')
else:
    component.add_comp_deps('kernel/cli')


