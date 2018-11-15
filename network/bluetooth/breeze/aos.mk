
NAME := breeze

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_SOURCES-y += core/core.c
$(NAME)_SOURCES-y += core/transport.c
$(NAME)_SOURCES-y += core/ble_service.c
$(NAME)_SOURCES-y += core/sha256.c
$(NAME)_SOURCES-y += core/utils.c

GLOBAL_INCLUDES-y += api include hal/include

$(NAME)_COMPONENTS-y := chip_code

secure_adv ?= 1
ifeq ($(secure_adv), 1)
GLOBAL_DEFINES-y += CONFIG_AIS_SECURE_ADV
endif

btstack ?= zephyr
ifeq (zephyr, $(btstack))
$(NAME)_COMPONENTS-y += bluetooth.breeze.hal.ble
endif

$(NAME)_SOURCES-y += api/breeze_export.c

bz_en_awss ?= 1
ifeq ($(bz_en_awss), 1)
GLOBAL_DEFINES-y += EN_COMBO_NET
$(NAME)_SOURCES-y += core/extcmd.c
$(NAME)_SOURCES-y += api/breeze_awss_export.c
endif

bz_en_auth ?= 1
ifeq ($(bz_en_auth), 1)
GLOBAL_DEFINES-y += EN_AUTH
$(NAME)_SOURCES-y += core/auth.c
endif
