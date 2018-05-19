src =Split(''' 
    main.c
''')
component =aos_component('linuxapp', src)

dependencis =Split(''' 
    tools/cli
    kernel/yloop
''')
for i in dependencis:
    component.add_comp_deps(i)

if aos_global_config.get('LWIP') == 1:
    component.add_comp_deps("kernel/protocols/net")

if aos_global_config.get("BENCHMARKS")==1:
    component.add_comp_deps("tools/benchmarks")
    component.add_global_macros("CONFIG_CMD_BENCHMARKS")



