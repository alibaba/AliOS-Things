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
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/MQTTPacket)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/client)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/deprecated_linkkit)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/awss/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/dev_bind/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/dev_bind/os)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/dev_bind/os/product)

ADD_LIBRARY (iot_awss OBJECT
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_wifimgr.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_main.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/zconfig_ieee80211.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_enrollee.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_adha.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_aplist.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_smartconfig.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/zconfig_vendor_common.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/zconfig_utils.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_aha.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/zconfig_protocol.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_crypt.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_wps.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_ht40.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_registrar.c
    ${EXTRA_C_SOURCES}
)

