src = Split('''
   sdk-impl.c
''')

component = aos_component('libsdk-impl', src)

if aos_global_config.compiler == 'gcc':
    component.add_cflags('-Wno-unused-function')
    component.add_cflags('-Wno-implicit-function-declaration')
    component.add_cflags('-Wno-unused-function') 

component.add_includes('.', './imports','./exports','../LITE-utils','../LITE-log','../guider','../iotx-system')

component.add_component_dependencis('utility/iotx-utils/guider')
