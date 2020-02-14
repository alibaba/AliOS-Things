NAME := lorawan_4_4_0

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := LoRaWAN Protocal Stack Version 4.4.0

$(NAME)_SOURCES  = mac/region/Region.c       \
                   mac/region/RegionCommon.c \
                   mac/LoRaMac.c             \
                   mac/LoRaMacCrypto.c       \
                   system/crypto/aes.c       \
                   system/crypto/cmac.c      \

ifneq ($(manufactory_lora_driver), 1)
$(NAME)_SOURCES += system/timer.c            \
                   system/delay.c            \
                   system/gpio.c
endif

ifeq ($(LORACHIP), sx1276)
$(NAME)_SOURCES += radio/sx1276/sx1276.c
$(NAME)_INCLUDES += radio/sx1276
endif

$(NAME)_INCLUDES +=  .             \
                    system/crypto \
                    radio         \
                    mac           \
                    mac/region    \
                    system

GLOBAL_DEFINES += CONFIG_NO_TCPIP

$(NAME)_SOURCES += linkwan/region/RegionCN470A.c \
                   linkwan/linkwan.c

$(NAME)_INCLUDES += linkwan/include \
                    linkwan/region

$(NAME)_SOURCES-$(CONFIG_LINKWAN_AT) += linkwan/linkwan_ica_at.c

