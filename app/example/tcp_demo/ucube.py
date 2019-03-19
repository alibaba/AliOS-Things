src =Split(''' 
    tcp_client.c
''')
component =aos_component('tcp_client', src)

dependencis =Split(''' 
    network/netmgr
''')
for i in dependencis:
    component.add_comp_deps(i)





