IF (WIN32)
    SET (OS_DIR win7)
ELSE (WIN32)
    SET (OS_DIR ubuntu)
ENDIF (WIN32)
FILE (GLOB EXTRA_C_SOURCES ${PROJECT_SOURCE_DIR}/src/platform/os/${OS_DIR}/*.c)

INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/platform/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/platform/os)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/platform/os/macos)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/platform/os/ubuntu)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/platform/os/win7)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/platform/ssl)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/platform/ssl/itls)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/platform/ssl/mbedtls)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/platform/ssl/openssl)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/exports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/sdk-impl/imports)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib/common/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib/common/itls)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib/common/tfs)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tls/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/include/mbedtls)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/misc)

ADD_LIBRARY (iot_platform STATIC
    ${PROJECT_SOURCE_DIR}/src/platform/ssl/mbedtls/HAL_DTLS_mbedtls.c
    ${PROJECT_SOURCE_DIR}/src/platform/ssl/mbedtls/HAL_TLS_mbedtls.c
    ${PROJECT_SOURCE_DIR}/src/platform/ssl/itls/HAL_TLS_itls.c
    ${EXTRA_C_SOURCES}
)

