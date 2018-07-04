INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT)
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

ADD_LIBRARY (iot_mqtt OBJECT
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/mqtt_client.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/mqtt_instance.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/MQTTPacket/MQTTPacket.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/MQTTPacket/MQTTSerializePublish.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/MQTTPacket/MQTTSubscribeClient.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/MQTTPacket/MQTTDeserializePublish.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/MQTTPacket/MQTTUnsubscribeClient.c
    ${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/MQTTPacket/MQTTConnectClient.c
    ${EXTRA_C_SOURCES}
)

