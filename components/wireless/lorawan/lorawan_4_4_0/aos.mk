NAME := lorawan_4_4_0

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := LoRaWAN Protocal Stack Version 4.4.0

$(NAME)_COMPONENTS += osal_aos

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

linkwan ?= 1
ifeq ($(linkwan), 1)
GLOBAL_DEFINES += CONFIG_LINKWAN CONFIG_DEBUG_LINKWAN     \
                  CONFIG_AOS_DISABLE_TICK CONFIG_NO_TCPIP \
                  REGION_CN470A CONFIG_NO_TCPIP           \
                  LOW_POWER_DISABLE
$(NAME)_SOURCES += linkwan/region/RegionCN470A.c \
                   linkwan/linkwan.c

$(NAME)_INCLUDES += linkwan/include \
                    linkwan/region

linkwanat ?= 0
ifeq ($(linkwanat), 1)
GLOBAL_DEFINES += CONFIG_LINKWAN_AT
$(NAME)_SOURCES += linkwan/linkwan_ica_at.c
#$(NAME)_SOURCES += linkwan/linkwan_at.c
endif
else
$(NAME)_SOURCES += mac/region/RegionAS923.c    \
                   mac/region/RegionAU915.c    \
                   mac/region/RegionCN470.c    \
                   mac/region/RegionCN779.c    \
                   mac/region/RegionEU433.c    \
                   mac/region/RegionEU868.c    \
                   mac/region/RegionIN865.c    \
                   mac/region/RegionKR920.c    \
                   mac/region/RegionUS915.c    \
                   mac/region/RegionUS915-Hybrid.c
endif
