src = Split('''
    netmgr_test.c
''')

component = aos_component('netmgr_test', src)

if aos_global_config.board == 'linuxhost':
    component.add_global_macros('CONFIG_AOS_NETMGRYTS_NOSMARTCONFIG')