src = Split('''
    service_manager.c
''')

component = aos_component('alink', src)

dependencis = Split('''
    framework/connectivity/wsf 
    utility/digest_algorithm 
    utility/cjson 
    utility/base64 
    utility/hashtable 
    utility/log 
    kernel/yloop 
    kernel/modules/fs/kv 
    framework/cloud 
    kernel/hal 
    security/mbedtls
''')

for d in dependencis:
    component.add_component_dependencis(d)

component.add_global_macro('CONFIG_ALINK')

component.add_global_includes('accs')
component.add_sources('accs/accs.c', 'accs/alink_protocol.c')

component.add_global_includes('cota')
component.add_sources('cota/cota.c')

component.add_global_includes('json')
component.add_sources('json/json_parser.c')

component.add_global_includes('os')
component.add_includes('os/product/', 'os/platform/')
component.add_sources('os/os_misc.c', 'os/os_thread.c', 'os/product/product.c')

if aos_global_config.board == 'linuxhost':
    component.add_sources('os/platform/aos_awss.c', 'os/platform/linux_hardware.c', 'os/platform/aos_os.c', 'os/platform/linux_ota.c', 'os/platform/aos_network.c','os/platform/aos_ssl.c')
else:
    component.add_sources('os/platform/aos_awss.c', 'os/platform/aos_hardware.c', 'os/platform/aos_os.c', 'os/platform/aos_network.c', 'os/platform/aos_ssl.c')

component.add_global_includes('system')
component.add_sources('system/alink_export.c', 'system/config.c', 'system/device.c')


