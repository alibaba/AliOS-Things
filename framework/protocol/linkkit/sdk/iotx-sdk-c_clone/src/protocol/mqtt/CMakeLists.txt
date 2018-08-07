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
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/MQTTPacket)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/client)

ADD_LIBRARY (iot_mqtt OBJECT
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/client/mqtt_client.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/client/mqtt_instance.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/MQTTPacket/MQTTPacket.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/MQTTPacket/MQTTSerializePublish.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/MQTTPacket/MQTTSubscribeClient.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/MQTTPacket/MQTTDeserializePublish.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/MQTTPacket/MQTTUnsubscribeClient.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/MQTTPacket/MQTTConnectClient.c
    ${EXTRA_C_SOURCES}
)

