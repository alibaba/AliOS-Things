src = Split('''
    ota_transport.c
    version_report.c
''')

component = aos_component('fota_mqtt_transport', src)

include_tmp = Split('''
    ..
    ../..
    ../../../mqtt/sdk-impl
    ../../../mqtt/platform    
''')

for i in include_tmp:
    component.add_global_includes(i)
    
if aos_global_config.get('vcall') == None:
    aos_global_config.set('vcall', 'rhino')

if aos_global_config.get('vcall') == 'rhino':
    component.add_macros("VCALL_RHINO") 
    component.add_comp_deps("framework/activation")