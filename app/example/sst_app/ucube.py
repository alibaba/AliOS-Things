src = Split('''
    sst_test.c
    sst_test_basic.c
    sst_test_performance.c
''')

component = aos_component('sst_test', src)

component.add_macros("CONFIG_AOS_SUPPORT=1")

dependencis = Split('''
    security/isst
    security/alicrypto
''')

for d in dependencis:
    component.add_comp_deps(d)
