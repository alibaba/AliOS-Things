src =Split(''' 
    gc0329.c
    camera_demo.c
''')

component = aos_component('camera_hal', src)

global_includes =Split(''' 
    include
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    DEVELOPERKIT_CAMERA
''')
for i in global_macros:
    component.add_global_macros(i)