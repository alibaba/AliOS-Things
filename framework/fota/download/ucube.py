src = Split('''
    download_common.c
''')

component = aos_component('fota_download', src)
component.add_includes('.')

comp_names = [comp.name for comp in aos_global_config.components]
if 'coap' in  comp_names:
    FOTA_DL_COAP=aos_global_config.get_aos_global_config('FOTA_DL_COAP')
    if FOTA_DL_COAP != None:
        component.add_component_dependencis('framework/fota/download/coap')
    else:
        component.add_component_dependencis('framework/fota/download/http')
else:
    component.add_component_dependencis('framework/fota/download/http')
