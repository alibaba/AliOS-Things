src =Split(''' 
   rtcp_yts_test.c
   rtp_yts_test.c 
''')
component =aos_component('rtp_yts_test', src)

dependencis =Split(''' 
    kernel/yloop
    kernel/cli
    network/netmgr
''')
for i in dependencis:
    component.add_comp_deps(i)





