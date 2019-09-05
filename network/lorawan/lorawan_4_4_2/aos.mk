NAME := lorawan_4_4_2

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := LoRaWAN Protocal Stack Version 4.4.2

$(NAME)_COMPONENTS += osal_aos

$(NAME)_SOURCES := peripherals/soft-se/aes.c      \
                   peripherals/soft-se/cmac.c     \
                   peripherals/soft-se/soft-se.c  \
                   mac/region/Region.c            \
                   mac/region/RegionCommon.c      \
                   mac/LoRaMac.c                  \
                   mac/LoRaMacCrypto.c            \
                   mac/LoRaMacAdr.c               \
                   mac/LoRaMacClassB.c            \
                   mac/LoRaMacCommands.c          \
                   mac/LoRaMacConfirmQueue.c      \
                   mac/LoRaMacCrypto.c            \
                   mac/LoRaMacFCntHandler.c       \
                   mac/LoRaMacParser.c            \
                   mac/LoRaMacSerializer.c


ifeq ($(LORACHIP), sx1276)
$(NAME)_SOURCES += radio/sx1276/sx1276.c  \
	               system/timer.c         \
				   system/delay.c         \
				   system/systime.c       \
				   system/gpio.c
$(NAME)_INCLUDES += radio/sx1276
endif

$(NAME)_INCLUDES +=  .             \
                    system/crypto \
                    radio         \
                    mac           \
                    mac/region    \
                    system        \
                    radio/sx1276  \
                    LoRaModuleSDK \
                    linkwan/include

GLOBAL_DEFINES += LORAMAC_CLASSB_ENABLED
#GLOBAL_DEFINES += LORAWAN_VERSION_110
GLOBAL_DEFINES += LOW_POWER_DISABLE

linkwan?=0
ifeq ($(linkwan), 1)
GLOBAL_DEFINES += CONFIG_LINKWAN
GLOBAL_DEFINES += CONFIG_DEBUG_LINKWAN
GLOBAL_DEFINES += CONFIG_AOS_DISABLE_TICK
GLOBAL_DEFINES += CONFIG_NO_TCPIP
GLOBAL_DEFINES += REGION_CN470
GLOBAL_DEFINES += CONFIG_NO_TCPIP
$(NAME)_SOURCES += mac/region/RegionCN470.c
$(NAME)_SOURCES += linkwan/linkwan.c


linkwanat ?= 0
ifeq ($(linkwanat), 1)
GLOBAL_DEFINES += CONFIG_LINKWAN_AT
GLOBAL_DEFINES += LOW_POWER_DISABLE
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
                   mac/region/RegionUS915.c
endif

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/gcc/LoRaMacMulticast.o

