src =Split(''' 
    acapp.c
''')
component =aos_component('acapp', src)

dependencis =Split(''' 
    framework/activation
    kernel/yloop
    tools/cli
''')
for i in dependencis:
    component.add_comp_deps(i)





