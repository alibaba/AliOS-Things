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
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/alcs/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/MQTTPacket)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/client)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/awss/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/awss/os)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/awss/os/product)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/awss/utility)

ADD_LIBRARY (awss OBJECT
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_wifimgr.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_main.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_cmp_coap.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_notify.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/registrar.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/ntp.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_packet.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_timer.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/passwd.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/zconfig_ieee80211.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/zconfig_vendor_common.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/zconfig_utils.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_cmp_mqtt.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/enrollee.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/zconfig_protocol.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/sha256.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/awss_report.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/os/os_misc.c
    ${PROJECT_SOURCE_DIR}/src/services/awss/utility/log.c
    ${EXTRA_C_SOURCES}
)

