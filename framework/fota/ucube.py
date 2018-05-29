src = Split('''
    ota_service.c
    ota_util.c
    ota_update_manifest.c
    ota_version.c
''')
component = aos_component('fota', src)

dependencis = Split('''
    framework/fota/platform
    framework/fota/download  
    utility/digest_algorithm 
    utility/cjson 
''')
for i in dependencis:
    component.add_comp_deps(i)
    
component.add_global_includes('.')
component.add_global_macros('AOS_FOTA')

    