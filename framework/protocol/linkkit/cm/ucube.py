src =Split(''' 
    src/iotx_cm_api.c
    src/iotx_cm_common.c
    src/iotx_cm_log.c
    src/iotx_cm_ota.c
    src/iotx_local_conn_alcs.c
    src/iotx_cloud_conn_coap.c
    src/iotx_cloud_conn_http.c
    src/iotx_cloud_conn_mqtt.c
    src/iotx_cm_cloud_conn.c
    src/iotx_cm_local_conn.c
''')
component =aos_component('cm', src)

dependencis =Split(''' 
    framework/common
''')
for i in dependencis:
    component.add_comp_deps(i)

global_includes =Split(''' 
    ./
    ./inc
''')
for i in global_includes:
    component.add_global_includes(i)

global_macros =Split(''' 
    MQTT_DIRECT
    MQTT_COMM_ENABLED
    CM_SUPPORT_LOCAL_CONN
    OTA_SIGNAL_CHANNEL=1
    CMP_SUPPORT_MEMORY_MAGIC
    CM_VIA_CLOUD_CONN_MQTT
    CM_VIA_CLOUD_CONN
    NDEBUG
''')
for i in global_macros:
    component.add_global_macros(i)

includes =Split(''' 
    ../../protocol/alink-ilop/sdk-encap
    ../../protocol/alink-ilop/iotkit-system
    ../../protocol/linkkit/alcs/
''')
for i in includes:
    component.add_includes(i)

