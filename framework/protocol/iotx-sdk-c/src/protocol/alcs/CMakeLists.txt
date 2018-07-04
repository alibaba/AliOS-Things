INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/alcs/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/MQTTPacket)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/misc)

ADD_LIBRARY (iot_alcs STATIC
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPSerialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_mqtt.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPObserve.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_api.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_adapter.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPNetwork.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPExport.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_client.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPResource.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPServer.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPPlatform.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPDeserialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/CoAPMessage.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_server.c
    ${PROJECT_SOURCE_DIR}/src/protocol/alcs/alcs_coap.c
    ${EXTRA_C_SOURCES}
)

