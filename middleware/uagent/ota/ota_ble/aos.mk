NAME := ota_ble

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := ble device upgrade

$(NAME)_SOURCES +=           \
      ota_breeze.c           \
      ota_breeze_service.c   \
      ota_breeze_transport.c \
      ota_breeze_plat.c      \
      ota_breeze_os.c

GLOBAL_INCLUDES += .
GLOBAL_DEFINES += AOS_COMP_OTA_BLE
