src = Split('''
    tls_test.c
''')

component = aos_component('tls_test', src)
component.add_component_dependencis('security/mbedtls')