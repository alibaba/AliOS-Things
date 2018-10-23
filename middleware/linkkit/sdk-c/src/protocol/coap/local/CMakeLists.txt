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
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/coap/local/)

ADD_LIBRARY (iot_coap_local OBJECT
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/local/CoAPSerialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/local/CoAPObserve.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/local/CoAPPlatform.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/local/CoAPExport.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/local/CoAPNetwork.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/local/CoAPResource.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/local/CoAPServer.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/local/CoAPDeserialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/local/CoAPMessage.c
    ${EXTRA_C_SOURCES}
)

