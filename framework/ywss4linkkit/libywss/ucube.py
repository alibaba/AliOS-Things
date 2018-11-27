src =Split(''' 
    awss.c
    awss_cmp_mqtt.c
    awss_report.c
    registrar.c
    zconfig_protocol.c
    awss_packet.c
    awss_main.c
    awss_wifimgr.c
    sha256.c
    zconfig_utils.c
    awss_cmp_coap.c
    awss_notify.c
    enrollee.c
    passwd.c
    zconfig_ieee80211.c
    zconfig_vendor_common.c
    os/os_misc.c
    utility/log.c
''')
component =aos_component('libywss', src)

dependencis =Split(''' 
    utility/digest_algorithm
    framework/protocol/alink-ilop
    framework/connectivity/link-coap
    framework/connectivity/mqtt
''')
for i in dependencis:
    component.add_comp_deps(i)


global_macros =Split(''' 
    CONFIG_YWSS
''')
for i in global_macros:
    component.add_global_macros(i)

includes =Split(''' 
    .
    ./os/
    ./os/product/
    ./os/platform/
    ./utility/
''')
for i in includes:
    component.add_includes(i)

macros =Split(''' 
    USE_LPTHREAD
    DEBUG
''')
for i in macros:
    component.add_macros(i)

if aos_global_config.get('awss_ble') == 1:
    component.add_sources('awss_blefi.c', 'blefi_config.c')
    component.add_macros('CONFIG_AWSS_BLE')
    
if aos_global_config.get('simulate_netmgr') == 1:
    macro_tmp = Split('''
        SIMULATE_NETMGR
    ''')
    macro_tmp.append( 'NETMGR_SSID=\\"' + 'Yuemewifi-3766' + '\\"' )
    macro_tmp.append( 'NETMGR_PW=\\"' + 'aos12345' + '\\"' )
    for i in macro_tmp:
        component.add_global_macros(i)

if aos_global_config.get('auto_netmgr') == 0:
    component.add_global_macros('AUTO_NETMGR_DISABLE')
    
    
    
    
    
    
    
    
    
    
    
    