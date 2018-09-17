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

ADD_LIBRARY (iot_alcs OBJECT
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPSerialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_mqtt.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPObserve.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_api.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPPlatform.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_adapter.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPExport.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_client.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPNetwork.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPResource.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPServer.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPDeserialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPMessage.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_server.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_coap.c
    ${EXTRA_C_SOURCES}
)

