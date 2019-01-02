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
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/example)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/example)

ADD_LIBRARY (iot_dm OBJECT
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_api.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_cm_wrapper.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_conn.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_cota.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_dispatch.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_fota.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_ipc.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_manager.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_message.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_message_cache.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_opt.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_ota.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_shadow.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_subscribe.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_tsl_alink.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_utils.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/linkkit_gateway_legacy.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/linkkit_solo_legacy.c
    ${EXTRA_C_SOURCES}
)

