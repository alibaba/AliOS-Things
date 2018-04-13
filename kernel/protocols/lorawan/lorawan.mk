
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
                   lora/mac/LoRaMacCrypto.c    \
                   ../../../device/lora/eml3047_lrwan/eml3047.c    \
                   ../../../device/lora/sx1276/sx1276.c

GLOBAL_INCLUDES +=  . \
                    ../../../device/lora/eml3047_lrwan    \
                    ../../../device/lora/sx1276   \
                    lora/system/crypto \
                    lora/radio       \
                    lora/mac         \
                    lora/mac/region  \
                    lora/system

$(NAME)_INCLUDES := \
../../../board/eml3047/inc \
../../../platform/mcu/stm32l0xx/Drivers/STM32L0xx_HAL_Driver/Inc \
../../../platform/mcu/stm32l0xx/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy \
../../../platform/mcu/stm32l0xx/Drivers/CMSIS/Device/ST/STM32L0xx/Include \
../../../platform/mcu/stm32l0xx/Drivers/CMSIS/Include

$(NAME)_DEFINES := \
USE_HAL_DRIVER \
STM32L071xx

linklora?=0
ifeq ($(linklora), 1)
GLOBAL_DEFINES += CONFIG_LINKLORA
GLOBAL_DEFINES += CONFIG_DEBUG_LINKLORA
$(NAME)_SOURCES += linklora/region/RegionCN470S.c
$(NAME)_SOURCES += linklora/linklora_class_a.c

GLOBAL_INCLUDES +=  linklora
GLOBAL_INCLUDES +=  linklora/region
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
