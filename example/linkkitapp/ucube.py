src =Split('''
    linkkit_sample_single.c
    linkkit_entry.c
''')
#linkkit_sample_gateway.c
component =aos_component('linkkitapp', src)

dependencis =Split('''
    framework/protocol/iotx-rhino/iotx-sdk-c
    framework/protocol/iotx-rhino/hal
    framework/netmgr
    framework/common
    framework/ywss4linkkit
    utility/cjson
    tools/cli
''')
for i in dependencis:
    component.add_comp_deps(i)

global_macros =Split('''
    MQTT_DIRECT
    ALIOT_DEBUG
    IOTX_DEBUG
    USE_LPTHREAD
    CONFIG_DM_DEVTYPE_SINGLE
    TEST_ALCS
    CONFIG_AOS_CLI
    MBEDTLS_SSL_MAX_CONTENT_LEN=6144
''')
for i in global_macros:
    component.add_global_macros(i)

if aos_global_config.get('LWIP') == 1:
    component.add_comp_deps("kernel/protocols/net")
    aos_global_config.set('no_with_lwip', 0)

