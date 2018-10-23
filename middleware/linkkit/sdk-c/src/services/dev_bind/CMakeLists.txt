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
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/MQTTPacket)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/client)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/deprecated_linkkit)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/awss/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/dev_bind/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/dev_bind/os)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/dev_bind/os/product)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/linkkit/dev_reset/)

ADD_LIBRARY (iot_bind OBJECT
    ${PROJECT_SOURCE_DIR}/src/services/dev_bind/awss_bind.c
    ${PROJECT_SOURCE_DIR}/src/services/dev_bind/awss_event.c
    ${PROJECT_SOURCE_DIR}/src/services/dev_bind/awss_notify.c
    ${PROJECT_SOURCE_DIR}/src/services/dev_bind/awss_timer.c
    ${PROJECT_SOURCE_DIR}/src/services/dev_bind/passwd.c
    ${PROJECT_SOURCE_DIR}/src/services/dev_bind/awss_cmp_coap.c
    ${PROJECT_SOURCE_DIR}/src/services/dev_bind/awss_cmp_mqtt.c
    ${PROJECT_SOURCE_DIR}/src/services/dev_bind/sha256.c
    ${PROJECT_SOURCE_DIR}/src/services/dev_bind/awss_report.c
    ${PROJECT_SOURCE_DIR}/src/services/dev_bind/awss_packet.c
    ${PROJECT_SOURCE_DIR}/src/services/dev_bind/os/os_misc.c
    ${EXTRA_C_SOURCES}
)

