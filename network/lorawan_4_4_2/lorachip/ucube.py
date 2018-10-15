src =Split(''' 
    sx1276/sx1276.c
    eml3047_lrwan/eml3047.c
''')
component =aos_component('lora ', src)


global_includes =Split(''' 
    .
    sx1276
    eml3047_lrwan
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    WITH_LORA
''')
for i in global_macros:
    component.add_global_macros(i)



