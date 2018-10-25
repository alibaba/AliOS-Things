INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/system)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/system/facility)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils/misc)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/alcs)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/alcs/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/coap)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/coap/cloud)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/coap/local)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/http)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/http2)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/MQTTPacket)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/client)

ADD_LIBRARY (iot_alcs OBJECT
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_mqtt.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_api.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_adapter.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_client.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_server.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_coap.c
    ${EXTRA_C_SOURCES}
)

