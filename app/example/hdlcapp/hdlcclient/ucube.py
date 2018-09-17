src     = Split('''
        hdlcclient.c
''')

component = aos_component('hdlcclient', src)
component.add_comp_deps('kernel/yloop', 'tools/cli', 'network/sal/atparser', 'network/netmgr')
component.add_global_macros('DEBUG', 'AOS_ATCMD')

component.add_global_includes('.')
