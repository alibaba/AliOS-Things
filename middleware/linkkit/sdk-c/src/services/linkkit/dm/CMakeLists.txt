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
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/coap)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/coap/cloud)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/coap/local)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/http)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/http2)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/http2/nghttp2)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/MQTTPacket)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/client)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/awss)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/dev_bind)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/dev_bind/os)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/dev_bind/os/product)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/http2_stream)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/cm/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dev_reset)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/client)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/server)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/ntp)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/mal)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/mal/ica)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/mal/ica/test)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/sal)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/sal/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/sal/include/internal)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/mdal/sal/src)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/ota)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/ota/impl)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/ota/prot)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/shadow)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/subdev)

ADD_LIBRARY (iot_dm OBJECT
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_api.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_cota.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_fota.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_ipc.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_manager.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_message.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_message_cache.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_msg_process.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_opt.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_ota.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_shadow.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_tsl_alink.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/dm_utils.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/server/dm_server_adapter.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/server/dm_server.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/client/dm_client_adapter.c
    ${PROJECT_SOURCE_DIR}/src/services/linkkit/dm/client/dm_client.c
    ${EXTRA_C_SOURCES}
)

