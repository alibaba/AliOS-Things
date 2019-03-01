NAME := ota_ble

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := ble device upgrade

$(NAME)_SOURCES += \
    src/ota_breeze.c \
    src/ota_breeze_service.c \
    src/ota_breeze_transport.c \
    src/ota_breeze_plat.c      \
    src/ota_breeze_os.c

GLOBAL_INCLUDES += . \
                   inc

GLOBAL_DEFINES += AOS_COMP_OTA_BLE
