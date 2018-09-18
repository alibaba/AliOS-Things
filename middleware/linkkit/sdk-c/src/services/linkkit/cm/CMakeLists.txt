INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/system)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils/misc)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/alcs/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/MQTTPacket)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/client)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/example)

ADD_LIBRARY (iot_cm OBJECT
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/iotx_cm_api.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/iotx_cm_cloud_conn.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/iotx_cm_common.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/iotx_cm_conn_coap.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/iotx_cm_connectivity.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/iotx_cm_conn_http.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/iotx_cm_conn_mqtt.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/iotx_cm_local_conn.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/iotx_cm_log.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/iotx_local_conn_alcs.c
    ${EXTRA_C_SOURCES}
)

