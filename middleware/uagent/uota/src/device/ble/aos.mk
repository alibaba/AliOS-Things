NAME := ota_ble

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := ble device upgrade

$(NAME)_SOURCES += \
    src/ota_breeze.c \
    src/ota_breeze_transport.c

$(NAME)_COMPONENTS += middleware.uagent.uota.hal

GLOBAL_INCLUDES += . \
                   inc

GLOBAL_DEFINES += OTA_BLE
#GLOBAL_DEFINES += OTA_BLE_LOG_INFO
#GLOBAL_DEFINES += OTA_BLE_LOG_DEBUG
#GLOBAL_DEFINES += OTA_BLE_LOG_ERR
