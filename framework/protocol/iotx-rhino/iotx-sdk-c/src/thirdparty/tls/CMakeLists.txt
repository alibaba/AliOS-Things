INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/lib)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tls/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/include/mbedtls)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/digest)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/log)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/utils/misc)

ADD_LIBRARY (iot_tls STATIC
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/pk.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/aes.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/entropy.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/pem.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/error.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/timing.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/rsa.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/cipher.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/ssl_tls.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/asn1parse.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/debug.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/oid.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/net_sockets.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/base64.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/pkparse.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/ssl_ciphersuites.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/ssl_cookie.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/pk_wrap.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/platform.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/cipher_wrap.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/sha1.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/ssl_cli.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/entropy_poll.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/md.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/md_wrap.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/sha256.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/x509_crt.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/ctr_drbg.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/bignum.c
    ${PROJECT_SOURCE_DIR}/src/thirdparty/tls/mbedtls-in-iotkit/library/x509.c
    ${EXTRA_C_SOURCES}
)

