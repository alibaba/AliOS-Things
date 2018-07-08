src = Split('''
    tfs_test.c
''')

component = aos_component('tfs_test', src)

dependencis = Split('''
    security/tfs 
    security/libid2 
    security/libkm 
    security/plat_gen 
    utility/base64 
    utility/digest_algorithm
''')

for d in dependencis:
    component.add_comp_deps(d)