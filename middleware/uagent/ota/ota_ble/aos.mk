NAME := ota_ble

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := ble device upgrade

$(NAME)_SOURCES += \
    src/ota_breeze.c \
    src/ota_breeze_service.c \
    src/ota_breeze_transport.c \
    src/ota_breeze_plat.c

$(NAME)_SOURCES += ../hal/ota_hal_module.c \
                   ../hal/ota_hal_os.c \
                   ../hal/ota_hal_plat.c

GLOBAL_INCLUDES += . \
                   inc \
                   ../include \
                   ../hal

GLOBAL_DEFINES += OTA_BLE
GLOBAL_DEFINES += OTA_ALIOS
GLOBAL_DEFINES += OTA_DISABLE_NETWORK
