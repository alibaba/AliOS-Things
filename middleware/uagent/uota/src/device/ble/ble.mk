NAME := ota_ble
$(info $(NAME))
$(NAME)_TYPE := framework

$(NAME)_SOURCES += \
    src/ota_breeze.c \
    src/ota_breeze_service.c \
    src/ota_breeze_transport.c

$(NAME)_COMPONENTS += middleware.uagent.uota.hal

GLOBAL_INCLUDES += . \
                   inc

GLOBAL_DEFINES += OTA_BLE
