src = Split('''
    ota_platform_os.c
''')

component = aos_component('fota_platform', src)
component.add_includes('.')

@post_config
def fota_platform_post_config(component):
    comp_names = [comp.name for comp in aos_global_config.components]
    if 'alink' in comp_names:
        component.add_comp_deps('framework/fota/platform/alink')
    elif 'mqtt' in comp_names:
        component.add_comp_deps('framework/fota/platform/mqtt')
    elif 'coap' in comp_names:
        component.add_comp_deps('framework/fota/platform/coap')
    else:
        component.add_comp_deps('framework/fota/platform/common')


fota_platform_post_config(component)
