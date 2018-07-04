INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/Link-MQTT/MQTTPacket)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/digest/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/misc)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/misc/)

ADD_LIBRARY (iot_utils OBJECT
    ${PROJECT_SOURCE_DIR}/src/utils/log/lite-log.c
    ${PROJECT_SOURCE_DIR}/src/utils/misc/linked_list.c
    ${PROJECT_SOURCE_DIR}/src/utils/misc/json_token.c
    ${PROJECT_SOURCE_DIR}/src/utils/misc/utils_list.c
    ${PROJECT_SOURCE_DIR}/src/utils/misc/json_parser.c
    ${PROJECT_SOURCE_DIR}/src/utils/misc/string_utils.c
    ${PROJECT_SOURCE_DIR}/src/utils/misc/class_interface.c
    ${PROJECT_SOURCE_DIR}/src/utils/misc/utils_epoch_time.c
    ${PROJECT_SOURCE_DIR}/src/utils/misc/utils_httpc.c
    ${PROJECT_SOURCE_DIR}/src/utils/misc/mem_stats.c
    ${PROJECT_SOURCE_DIR}/src/utils/misc/utils_timer.c
    ${PROJECT_SOURCE_DIR}/src/utils/misc/lite_queue.c
    ${PROJECT_SOURCE_DIR}/src/utils/misc/lite-cjson.c
    ${PROJECT_SOURCE_DIR}/src/utils/misc/utils_net.c
    ${PROJECT_SOURCE_DIR}/src/utils/digest/utils_hmac.c
    ${PROJECT_SOURCE_DIR}/src/utils/digest/utils_md5.c
    ${PROJECT_SOURCE_DIR}/src/utils/digest/utils_sha256.c
    ${PROJECT_SOURCE_DIR}/src/utils/digest/utils_base64.c
    ${PROJECT_SOURCE_DIR}/src/utils/digest/utils_sha1.c
    ${EXTRA_C_SOURCES}
)

