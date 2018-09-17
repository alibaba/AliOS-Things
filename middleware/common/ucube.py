src = Split('''
    main.c 
    version.c
''')

component = aos_component('framework', src)
component.add_global_includes('include')

CONFIG_SYSINFO_APP_VERSION = 'app-1.0.0-'+aos_global_config.current_time
print( 'app_version:', CONFIG_SYSINFO_APP_VERSION)
macro_tmp = Split('''
    AOS_FRAMEWORK_COMMON
''')
macro_tmp.append( 'SYSINFO_APP_VERSION=\\"' + CONFIG_SYSINFO_APP_VERSION + '\\"' )

if aos_global_config.get('osal') == 'winmsvs':
    comp_names = [comp.name for comp in aos_global_config.components]
    if 'fota' in  comp_names:
        print('function aos_get_os_version is already defined in module fota')
    else:
        #No aos_get_os_version define, use myself.
        component.add_macros("USE_MY_aos_get_os_version")   
 
for i in macro_tmp:
    component.add_global_macros(i)


