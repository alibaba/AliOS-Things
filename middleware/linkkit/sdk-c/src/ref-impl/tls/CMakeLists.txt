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
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/ref-impl/tls/)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/ref-impl/tls/include)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/ref-impl/tls/include/mbedtls)
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library)

ADD_LIBRARY (iot_tls STATIC
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/pk.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/aes.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/entropy.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/error.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/pk_wrap.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/timing.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/rsa.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/cipher.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/ssl_tls.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/asn1parse.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/debug.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/net_sockets.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/base64.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/pkparse.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/ssl_ciphersuites.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/pem.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/ssl_cookie.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/platform.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/sha1.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/oid.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/entropy_poll.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/md.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/md_wrap.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/sha256.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/ssl_cli.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/x509_crt.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/md5.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/ctr_drbg.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/cipher_wrap.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/bignum.c
    ${PROJECT_SOURCE_DIR}/src/ref-impl/tls/library/x509.c
    ${EXTRA_C_SOURCES}
)

