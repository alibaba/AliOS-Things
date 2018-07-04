INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/MQTTPacket)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/inc)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/inc/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/src)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/include/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/misc)

ADD_LIBRARY (iot_linkkit OBJECT
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/linkkit_legacy_adaptor.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/packet.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/mempool.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/iotx_dm_manager.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/linkkit_api.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/iotx_dm_message.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/iotx_dm_msg_dispatch.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/queue.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/iotx_dm_api.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/iotx_dm_cm_wrapper.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/iotx_dm_common.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/tmpmsg.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/iotx_dm_message_cache.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/iotx_dm_ipc.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/thread.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/iotx_dm_shadow.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/watcher.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/linkkit_gw_api.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/src/iotx_dm_tsl_alink.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/src/iotx_cm_conn_mqtt.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/src/iotx_cm_local_conn.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/src/iotx_cm_conn_coap.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/src/iotx_cm_conn_http.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/src/iotx_cm_cloud_conn.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/src/iotx_cm_api.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/src/iotx_cm_log.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/src/iotx_cm_common.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/src/iotx_local_conn_alcs.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/src/iotx_cm_connectivity.c
    ${EXTRA_C_SOURCES}
)

