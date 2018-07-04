INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/MQTTPacket)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/inc)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/inc/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/src)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/misc)

ADD_LIBRARY (iot_cmp OBJECT
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

