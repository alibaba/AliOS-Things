src = Split('''
    sst_test.c
''')

component = aos_component('sst_test', src)

dependencis = Split('''
    security/libsst 
    security/libkm 
    security/plat_gen 
    security/alicrypto 
''')

for d in dependencis:
    component.add_comp_deps(d)