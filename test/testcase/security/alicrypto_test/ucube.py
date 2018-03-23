src = Split('''
    alicrypto_test.c
''')

component = aos_component('alicrypto_test', src)
component.add_component_dependencis('security/alicrypto')
