src     = Split('''
        mbedtls_test.c
        tls_client.c
''')

component = aos_component('mbedtls_test', src)
component.add_comp_deps('network/yloop', 'tools/cli', 'utility/mbedtls', 'network/netmgr', 'middleware/uagent/ulog')
component.add_global_macros('CONFIG_MBEDTLS_TEST')
