src =Split('''
    id2_client.c
    id2_client_demo.c
''')
component =aos_component('id2_client', src)

deps =Split('''
    security/id2
    security/irot
    security/alicrypto
''')


for x in deps:
    component.add_comp_deps(x)

