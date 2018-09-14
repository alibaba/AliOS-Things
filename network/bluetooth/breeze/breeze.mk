
NAME := breeze

$(NAME)_MBINS_TYPE := kernel

$(NAME)_SOURCES := api/ali_export.c

$(NAME)_SOURCES += core/ali_auth.c
$(NAME)_SOURCES += core/sha256.c
$(NAME)_SOURCES += core/ali_core.c
$(NAME)_SOURCES += core/ali_gap.c
$(NAME)_SOURCES += core/ali_transport.c
$(NAME)_SOURCES += core/ali_ext.c
$(NAME)_SOURCES += core/ble_ais.c

ifeq ($(breeze_ota),1)
$(NAME)_SOURCES += core/ali_ota.c
endif

enhanced_auth ?= 1
ifeq ($(enhanced_auth),1)
GLOBAL_DEFINES += CONFIG_ENHANCED_AUTH
GLOBAL_DEFINES += CONFIG_AES128_CBC
else
$(NAME)_SOURCES += ali_transport/aes.c
GLOBAL_DEFINES += CONFIG_AES128_ECB
endif

GLOBAL_INCLUDES += api include hal/include

$(NAME)_COMPONENTS := digest_algorithm activation chip_code

secure_adv ?= 1
ifeq ($(secure_adv),1)
GLOBAL_DEFINES += CONFIG_AIS_SECURE_ADV
endif

btstack ?= zephyr
ifeq (zephyr, $(btstack))
$(NAME)_COMPONENTS += bluetooth.breeze.hal.ble
endif

breeze_awss ?= 1
ifeq ($(breeze_awss),1)
$(NAME)_SOURCES += api/ali_ywss_export.c
endif
