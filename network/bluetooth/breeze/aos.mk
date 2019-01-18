
NAME := breeze

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := breeze provides secure BLE connection to Alibaba IoT cloud and services.

$(NAME)_SOURCES-y += core/core.c
$(NAME)_SOURCES-y += core/transport.c
$(NAME)_SOURCES-y += core/ble_service.c
$(NAME)_SOURCES-y += core/utils.c
$(NAME)_SOURCES-y += core/extcmd.c

GLOBAL_INCLUDES-y += api include hal/include

$(NAME)_COMPONENTS-y := chip_code

bz_secure_adv ?= 0
ifeq ($(bz_secure_adv), 1)
GLOBAL_DEFINES-y += CONFIG_AIS_SECURE_ADV
endif

bz_model_encry ?= 0
ifeq ($(bz_model_encry), 1)
GLOBAL_DEFINES-y += CONFIG_MODEL_SEC
endif

btstack ?= zephyr
ifeq (zephyr, $(btstack))
$(NAME)_COMPONENTS-y += breeze_hal
endif

$(NAME)_SOURCES-y += api/breeze_export.c

bz_en_auth ?= 1
ifeq ($(bz_en_auth), 1)
GLOBAL_DEFINES-y += EN_AUTH
$(NAME)_SOURCES-y += core/auth.c
endif

bz_en_awss ?= 1
ifeq ($(bz_en_awss), 1)
ifeq ($(bz_en_auth), 0)
$(error awss need authentication, please set "bz_en_auth = 1")
endif
GLOBAL_DEFINES-y += EN_COMBO_NET
$(NAME)_SOURCES-y += api/breeze_awss_export.c
endif

