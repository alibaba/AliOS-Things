src =Split(''' 
    base/log/LITE-log/lite-log.c
    base/utils/LITE-utils/src/json_parser.c
    base/utils/LITE-utils/src/json_token.c
    base/utils/LITE-utils/src/mem_stats.c
    base/utils/LITE-utils/src/string_utils.c
    base/utils//misc/utils_epoch_time.c
    base/utils//misc/utils_httpc.c
    base/utils//misc/utils_list.c
    base/utils//misc/utils_net.c
    base/utils//misc/utils_timer.c
    base/utils//misc/work_queue.c
    base/utils//digest/utils_base64.c
    base/utils//digest/utils_hmac.c
    base/utils//digest/utils_md5.c
    base/utils//digest/utils_sha1.c
    hal-impl/rhino/HAL_OS_rhino.c
    hal-impl/rhino/HAL_TCP_rhino.c
    hal-impl/rhino/HAL_PRODUCT_rhino.c
    hal-impl/rhino/HAL_UDP_rhino.c
    hal-impl/tls/mbedtls/HAL_DTLS_mbedtls.c
    hal-impl/tls/mbedtls/HAL_TLS_mbedtls.c
    iotkit-system/src/sdk-impl.c
    iotkit-system/src/ca.c
    iotkit-system/src/device.c
    iotkit-system/src/guider.c
    iotkit-system/src/id2_guider.c
    iotkit-system/src/report.c
    layers/config/src/dev_conf.c
''')
component =aos_component('alink-ilop', src)

dependencis =Split(''' 
    security/mbedtls
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
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split(''' 
    sdk-encap
    sdk-encap/imports
    iotkit-system
    base/log/LITE-log
    base/utils
    base/utils/include
    layers/config/include
    iotkit-system
    base/utils/LITE-utils/src/
    base/utils/misc/
    base/utils/digest
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    CONFIG_ALINK_ILOP
''')
for i in global_macros:
    component.add_global_macros(i)

includes =Split(''' 
    base/log/LITE-log/
    ./ilop_utils_config.h
    ./exports
    ../../sdk-encap
    ../log
    ./
    ./
    layers/config/include
''')
for i in includes:
    component.add_includes(i)


cflags =Split(''' 
    -DUTILS_printf=HAL_Printf
    -DCONFIG_GUIDER_AUTH_TIMEOUT=500
    -DMQTT_COMM_ENABLED
''')
for i in cflags:
    component.add_cflags(i)

component.add_global_macros('LITE_UTILS_CONFIG_H=\\"'+'ilop_utils_config.h'+'\\"')