src = Split('''
    main.c 
    version.c
''')

component = aos_component('framework', src)

CONFIG_SYSINFO_APP_VERSION = 'app-1.0.0-'+aos_global_config.current_time
print( 'app_version:', CONFIG_SYSINFO_APP_VERSION)
macro_tmp = Split('''
    AOS_FRAMEWORK_COMMON
''')
macro_tmp.append( 'SYSINFO_APP_VERSION=\\"' + CONFIG_SYSINFO_APP_VERSION + '\\"' )

for i in macro_tmp:
    component.add_macros(i)


