src =Split(''' 
    itls_client.c
''')
component =aos_component('itls_client', src)

dependencis =Split(''' 
    security/itls
    security/id2
    security/irot/km
    security/alicrypto
    network/netmgr
''')
for i in dependencis:
    component.add_comp_deps(i)





