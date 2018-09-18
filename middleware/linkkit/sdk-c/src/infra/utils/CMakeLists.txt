INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/log/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/infra/utils/misc)

ADD_LIBRARY (iot_utils OBJECT
    ${PROJECT_SOURCE_DIR}/src/infra/utils/misc/linked_list.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/misc/json_parser.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/misc/string_utils.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/misc/json_token.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/misc/utils_epoch_time.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/misc/mem_stats.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/misc/utils_event.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/misc/utils_httpc.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/misc/utils_list.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/misc/utils_timer.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/misc/lite-cjson.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/misc/utils_net.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/digest/utils_hmac.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/digest/utils_md5.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/digest/utils_sha256.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/digest/utils_base64.c
    ${PROJECT_SOURCE_DIR}/src/infra/utils/digest/utils_sha1.c
    ${EXTRA_C_SOURCES}
)

