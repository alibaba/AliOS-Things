NAME := lorawan

$(NAME)_SOURCES := lora/system/crypto/aes.c                \
                   lora/system/crypto/cmac.c               \
                   lora/system/timeServer.c      \
                   lora/system/low_power.c       \
                   lora/system/utilities.c       \
                   lora/system/delay.c           \
                   lora/mac/region/Region.c         \
                   lora/mac/region/RegionCommon.c   \
                   lora/mac/LoRaMac.c               \
                   lora/mac/LoRaMacCrypto.c

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
GLOBAL_DEFINES += REGION_CN470A
$(NAME)_SOURCES += linkwan/region/RegionCN470A.c
$(NAME)_SOURCES += linkwan/linkwan.c

GLOBAL_INCLUDES += linkwan/include
GLOBAL_INCLUDES += linkwan/region

linkwanat ?= 0
ifeq ($(linkwanat), 1)
GLOBAL_DEFINES += CONFIG_LINKWAN_AT
$(NAME)_SOURCES += linkwan/linkwan_at.c
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
