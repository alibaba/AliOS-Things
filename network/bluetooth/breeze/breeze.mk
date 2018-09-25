
NAME := breeze

$(NAME)_MBINS_TYPE := kernel


$(NAME)_SOURCES += core/auth.c
$(NAME)_SOURCES += core/core.c
$(NAME)_SOURCES += core/transport.c
$(NAME)_SOURCES += core/ext.c
$(NAME)_SOURCES += core/ble_ais.c
$(NAME)_SOURCES += core/sha256.c

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

$(NAME)_SOURCES += api/breeze_export.c

breeze_awss ?= 0
ifeq ($(breeze_awss),1)
$(NAME)_SOURCES += api/breeze_awss_export.c
endif
