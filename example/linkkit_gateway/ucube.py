src =Split(''' 
    linkkit_sample_gateway.c
    light.c
    linkkit_entry.c
''')
#linkkit_sample_gateway.c
component =aos_component('linkkit_gateway', src)

dependencis =Split(''' 
    framework/protocol/linkkit/sdk
    framework/protocol/linkkit/hal
    framework/netmgr
    framework/common
    utility/cjson
    tools/cli
''')
for i in dependencis:
    component.add_comp_deps(i)

global_macros =Split(''' 
    MQTT_DIRECT
    ALIOT_DEBUG
    IOTX_DEBUG
    TEST_ALCS
    CONFIG_AOS_CLI
    USE_LPTHREAD
    CM_SUPPORT_MULTI_THREAD_VIA_HAL
    MBEDTLS_SSL_MAX_CONTENT_LEN=6144 
''')
for i in global_macros:
    component.add_global_macros(i)

if aos_global_config.get('LWIP') == 1:
    component.add_comp_deps("kernel/protocols/net")
    aos_global_config.set('no_with_lwip', 0)

