
NAME := breeze

$(NAME)_COMPONENTS-$(CONFIG_COMP_BZ_BUILTIN_HAL):= breeze_hal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := breeze provides secure BLE connection to Alibaba IoT cloud and services.

$(NAME)_SOURCES-y += api/breeze_export.c
$(NAME)_SOURCES-y += core/src/core.c
$(NAME)_SOURCES-y += core/src/transport.c
$(NAME)_SOURCES-y += core/src/ble_service.c
$(NAME)_SOURCES-y += core/src/utils.c
$(NAME)_SOURCES-y += core/src/extcmd.c

$(NAME)_INCLUDES += include \
                    core/include 
GLOBAL_INCLUDES += ../../../../../include/wireless/bluetooth/breeze


ifeq ($(EN_AUTH), y)
$(NAME)_SOURCES-y += core/src/auth.c
endif

ifeq ($(EN_COMBO_NET), y)
ifneq ($(EN_AUTH), y)
$(error awss need authentication, please set "bz_en_auth = 1")
endif
$(NAME)_SOURCES-y += api/breeze_awss_export.c
endif

