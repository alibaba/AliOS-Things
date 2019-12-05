src =Split(''' 
    sal_app.c
''')
component =aos_component('sal_app', src)

dependencis =Split(''' 
    kernel/yloop
    kernel/cli
    network/netmgr
    network/nal/sal
''')
for i in dependencis:
    component.add_comp_deps(i)





