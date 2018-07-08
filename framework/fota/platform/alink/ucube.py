src = Split('''
    ota_transport.c 
''')

component = aos_component('fota_alink_transport', src)

includes = Split('''
    ../../../protocol/alink/system
    ../../../protocol/alink/os
    ../..
    ..
''')

for i in includes:
    component.add_includes(i)
