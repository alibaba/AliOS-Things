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
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/coap/)

ADD_LIBRARY (iot_coap STATIC
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/CoAPSerialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/CoAPExport.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/CoAPNetwork.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/CoAPDeserialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/iotx_coap_api.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/CoAPMessage.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/iotx_ca_cert.c
    ${EXTRA_C_SOURCES}
)

