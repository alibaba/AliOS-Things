INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/coap/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/misc)

ADD_LIBRARY (iot_coap OBJECT
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/CoAPSerialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/CoAPExport.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/CoAPNetwork.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/CoAPDeserialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/iotx_coap_api.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/CoAPMessage.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/iotx_ca_cert.c
    ${EXTRA_C_SOURCES}
)

