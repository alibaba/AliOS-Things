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
    hal/ota_hal_os.c \
    hal/ota_hal_plat.c \
    hal/ota_hal_module.c \


HDR_REFS            += src/infra
HDR_REFS            += src/protocol/mqtt/client
HDR_REFS            += src/ref-impl/hal/os/$(CONFIG_VENDOR)
