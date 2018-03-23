src = Split('''
    ota_platform_os.c
''')

component = aos_component('fota_platform', src)
component.add_includes('.')

comp_names = [comp.name for comp in aos_global_config.components]
if 'alink' in  comp_names:
    component.add_component_dependencis('framework/fota/platform/alink')
elif 'mqtt' in  comp_names:
    component.add_component_dependencis('framework/fota/platform/mqtt')
elif 'coap' in  comp_names:
    component.add_component_dependencis('framework/fota/platform/coap')
else:
    component.add_component_dependencis('framework/fota/platform/common')