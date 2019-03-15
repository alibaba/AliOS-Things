src = Split('''
    id2_test.c
''')

component = aos_component('id2_test', src)

dependencis = Split('''
    security/id2
    security/irot
    security/alicrypto
''')

for d in dependencis:
    component.add_comp_deps(d)
