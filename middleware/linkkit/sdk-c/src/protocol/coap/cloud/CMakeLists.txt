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
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/coap/cloud/)

ADD_LIBRARY (iot_coap_cloud STATIC
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/cloud/CoAPSerialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/cloud/CoAPExport.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/cloud/CoAPNetwork.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/cloud/CoAPDeserialize.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/cloud/iotx_coap_api.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/cloud/CoAPMessage.c
    ${PROJECT_SOURCE_DIR}/src/protocol/coap/cloud/iotx_ca_cert.c
    ${EXTRA_C_SOURCES}
)

