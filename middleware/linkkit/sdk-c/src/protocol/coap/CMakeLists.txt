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
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/Cloud_CoAPSerialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/Cloud_CoAPExport.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/Cloud_CoAPNetwork.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/Cloud_CoAPDeserialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/iotx_coap_api.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/Cloud_CoAPMessage.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/iotx_ca_cert.c
    ${EXTRA_C_SOURCES}
)

