
NAME := breeze

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := breeze provides secure BLE connection to Alibaba IoT cloud and services.

$(NAME)_SOURCES-y += core/src/core.c
$(NAME)_SOURCES-y += core/src/transport.c
$(NAME)_SOURCES-y += core/src/ble_service.c
$(NAME)_SOURCES-y += core/src/utils.c
$(NAME)_SOURCES-y += core/src/extcmd.c

GLOBAL_INCLUDES-y += api include core/include 

$(NAME)_COMPONENTS-y := chip_code

ifeq ($(CONFIG_COMP_BZ_SECURE_ADV), y)
GLOBAL_DEFINES-y += CONFIG_AIS_SECURE_ADV
endif

ifeq ($(CONFIG_COMP_BZ_MODEL_SEC), y)
GLOBAL_DEFINES-y += CONFIG_MODEL_SECURITY
endif

$(NAME)_SOURCES-y += api/breeze_export.c

ifeq ($(CONFIG_COMP_BZ_EN_AUTH), y)
GLOBAL_DEFINES-y += EN_AUTH
$(NAME)_SOURCES-y += core/src/auth.c
ifeq ($(CONFIG_COMP_BZ_EN_AUTH_OFFLINE), y)
GLOBAL_DEFINES-y += EN_AUTH_OFFLINE
endif
endif

ifeq ($(CONFIG_COMP_BZ_EN_AWSS), y)
ifneq ($(CONFIG_COMP_BZ_EN_AUTH), y)
$(error awss need authentication, please set "bz_en_auth = 1")
endif
GLOBAL_DEFINES-y += EN_COMBO_NET
$(NAME)_SOURCES-y += api/breeze_awss_export.c
endif

