src =Split(''' 
    networkapp.c
''')
component =aos_component('networkapp', src)

if aos_global_config.get("BENCHMARKS")==1:
    component.add_comp_deps("tools/benchmarks")
    component.add_global_macros("CONFIG_CMD_BENCHMARKS")

dependencis =Split(''' 
    kernel/yloop
    tools/cli
    framework/netmgr
''')
for i in dependencis:
    component.add_comp_deps(i)





