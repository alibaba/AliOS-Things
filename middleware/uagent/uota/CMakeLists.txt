IF (WIN32)
    SET (OS_DIR win7)
ELSE (WIN32)
    SET (OS_DIR ubuntu)
ENDIF (WIN32)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/ref-impl/hal/os/${OS_DIR})

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
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/protocol/mqtt/client/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uOTA/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uOTA/hal)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uOTA/inc)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uOTA/src)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uOTA/src/download)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uOTA/src/otalib)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uOTA/src/service)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uOTA/src/transport)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uOTA/src/utility)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uOTA/src/utility/crc)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uOTA/src/verify)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uOTA/src/verify/hash)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/services/uOTA/src/verify/rsa)

ADD_LIBRARY (iot_ota OBJECT
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/otalib/ota.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/service/ota_service.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/service/ota_version.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/transport/ota_transport_mqtt.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/transport/ota_transport_coap.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/download/ota_manifest.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/download/ota_download_http.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/download/ota_download_coap.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/download/ota_socket.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/verify/ota_util.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/verify/ota_verify.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/verify/hash/ota_hash.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/verify/hash/ota_md5.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/verify/hash/ota_sha256.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/verify/rsa/ota_bignum.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/verify/rsa/ota_rsa.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/verify/rsa/ota_hash_rsa.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/src/utility/crc/crc.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/hal/ota_hal_os.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/hal/ota_hal_plat.c
    ${PROJECT_SOURCE_DIR}/src/services/uOTA/hal/ota_hal_module.c
    ${EXTRA_C_SOURCES}
)

