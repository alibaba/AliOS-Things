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

ADD_LIBRARY (iot_coap_local OBJECT
    ${PROJECT_SOURCE_DIR}/src/protocol/coap_local/CoAPSerialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap_local/CoAPObserve.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap_local/CoAPPlatform.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap_local/CoAPExport.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap_local/CoAPNetwork.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap_local/CoAPResource.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap_local/CoAPServer.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap_local/CoAPDeserialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap_local/CoAPMessage.c
    ${EXTRA_C_SOURCES}
)

