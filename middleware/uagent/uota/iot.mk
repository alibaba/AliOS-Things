LIBA_TARGET         := libiot_ota.a

LIB_SRCS_PATTERN    := \
    src/otalib/ota.c \
    src/service/ota_service.c \
    src/service/ota_version.c \
    src/transport/ota_transport_mqtt.c \
    src/transport/ota_transport_coap.c \
    src/download/ota_manifest.c \
    src/download/ota_download_http.c \
    src/download/ota_download_coap.c \
    src/download/ota_socket.c \
    src/verify/ota_util.c \
    src/verify/ota_verify.c \
    src/utility/chip_code/chip_code.c \
    src/utility/crc/crc.c \
    src/verify/ota_rsa_verify.c \
    src/verify/hash/ota_hash.c \
    src/verify/hash/ota_md5.c \
    src/verify/hash/ota_sha256.c \
    src/verify/rsa/ota_bignum.c \
    src/verify/rsa/ota_rsa.c \
    src/verify/rsa/ota_hash_rsa.c \
    src/utility/base64/ota_base64.c \
    src/utility/hexstr2hexbuf/ota_hexstr2hexbuf.c \
    hal/ota_hal_os.c \
    hal/ota_hal_plat.c \
    hal/ota_hal_module.c \


HDR_REFS            += src/infra
HDR_REFS            += src/protocol/mqtt/client
HDR_REFS            += src/ref-impl/hal/os/$(CONFIG_VENDOR)

define Extra_CMake_Head
    echo 'IF (WIN32)' $(1)
    echo '    SET (OS_DIR win7)' $(1)
    echo 'ELSE (WIN32)' $(1)
    echo '    SET (OS_DIR ubuntu)' $(1)
    echo 'ENDIF (WIN32)' $(1)
    echo 'INCLUDE_DIRECTORIES ($${PROJECT_SOURCE_DIR}/src/ref-impl/hal/os/$${OS_DIR})' $(1)
    echo '' $(1)
endef
