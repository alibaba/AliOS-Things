src = Split('''
    guider.c
''')
component = aos_component('libguider', src)

include_tmp = Split('''
    .
    ../iotx-system 
    ../LITE-utils 
    ../LITE-log 
    ../misc 
    ../digest 
    ../sdk-impl 
    ../../security/tfs
''')
for i in include_tmp:
    component.add_global_includes(i)

component.add_component_dependencis('utility/iotx-utils/iotx-system')

