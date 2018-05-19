src = Split('''
    alicrypto_test.c
''')

component = aos_component('alicrypto_test', src)
component.add_comp_deps('security/alicrypto')
