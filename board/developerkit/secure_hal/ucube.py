src =Split(''' 
    id2_client_demo.c
    secure_demo.c
''')

component = aos_component('secure_hal', src)

global_includes =Split(''' 
    include
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    DEVELOPERKIT_SECURE
''')
for i in global_macros:
    component.add_global_macros(i)