NAME := lorawan_4_4_2

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := lora/system/crypto/aes.c                \
                   lora/system/crypto/cmac.c               \
                   lora/system/crypto/soft-se.c            \
                   lora/system/timeServer.c      \
                   lora/system/low_power.c       \
                   lora/system/utilities.c       \
                   lora/system/delay.c           \
                   lora/system/systime.c           \
                   lora/mac/region/Region.c         \
                   lora/mac/region/RegionCommon.c   \
                   lora/mac/LoRaMac.c               \
                   lora/mac/LoRaMacCrypto.c         \
                   lora/mac/LoRaMacAdr.c            \
                   lora/mac/LoRaMacClassB.c         \
                   lora/mac/LoRaMacCommands.c       \
                   lora/mac/LoRaMacConfirmQueue.c       \
                   lora/mac/LoRaMacCrypto.c       \
                   lora/mac/LoRaMacFCntHandler.c       \
                   lora/mac/LoRaMacParser.c       \
                   lora/mac/LoRaMacSerializer.c

GLOBAL_INCLUDES +=  . \
                    lora/system/crypto \
                    lora/radio       \
                    lora/mac         \
                    lora/mac/region  \
                    lora/system

linkwan?=0
ifeq ($(linkwan), 1)
GLOBAL_DEFINES += CONFIG_LINKWAN
GLOBAL_DEFINES += CONFIG_DEBUG_LINKWAN
GLOBAL_DEFINES += CONFIG_AOS_DISABLE_TICK
GLOBAL_DEFINES += CONFIG_NO_TCPIP
GLOBAL_DEFINES += REGION_CN470A
GLOBAL_DEFINES += CONFIG_NO_TCPIP
$(NAME)_SOURCES += linkwan/region/RegionCN470A.c
$(NAME)_SOURCES += linkwan/linkwan.c

GLOBAL_INCLUDES += linkwan/include
GLOBAL_INCLUDES += linkwan/region

linkwanat ?= 0
ifeq ($(linkwanat), 1)
GLOBAL_DEFINES += CONFIG_LINKWAN_AT
GLOBAL_DEFINES += LOW_POWER_DISABLE
GLOBAL_DEFINES += LORAMAC_CLASSB_ENABLED
$(NAME)_SOURCES += linkwan/linkwan_ica_at.c
#$(NAME)_SOURCES += linkwan/linkwan_at.c
endif
else
$(NAME)_SOURCES += lora/mac/region/RegionAS923.c    \
                   lora/mac/region/RegionAU915.c    \
                   lora/mac/region/RegionCN470.c    \
                   lora/mac/region/RegionCN779.c    \
                   lora/mac/region/RegionEU433.c    \
                   lora/mac/region/RegionEU868.c    \
                   lora/mac/region/RegionIN865.c    \
                   lora/mac/region/RegionKR920.c    \
                   lora/mac/region/RegionUS915.c    \
                   lora/mac/region/RegionUS915-Hybrid.c
endif
