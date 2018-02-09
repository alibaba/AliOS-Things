
NAME := lorawan

$(NAME)_SOURCES := Lora/Crypto/aes.c                \
				   Lora/Crypto/cmac.c               \
				   Lora/Utilities/timeServer.c      \
				   Lora/Utilities/low_power.c       \
				   Lora/Utilities/utilities.c       \
				   Lora/Utilities/delay.c           \
                   Lora/Mac/region/Region.c         \
                   Lora/Mac/region/RegionAS923.c    \
                   Lora/Mac/region/RegionAU915.c    \
                   Lora/Mac/region/RegionCN470.c    \
                   Lora/Mac/region/RegionCN779.c    \
                   Lora/Mac/region/RegionCommon.c   \
                   Lora/Mac/region/RegionEU433.c    \
                   Lora/Mac/region/RegionEU868.c    \
                   Lora/Mac/region/RegionIN865.c    \
                   Lora/Mac/region/RegionKR920.c    \
                   Lora/Mac/region/RegionUS915.c    \
                   Lora/Mac/region/RegionUS915-Hybrid.c \
                   Lora/Mac/LoRaMac.c               \
                   Lora/Mac/LoRaMacCrypto.c    \
				   Lora/hal/lorawan_port.c    \
				   ../../../device/lora/eml3047_lrwan/eml3047.c    \
				   ../../../device/lora/sx1276/sx1276.c    \
				   # BSP/EML3047/eml3047.c              \
				   # BSP/sx1276/sx1276.c              \

GLOBAL_INCLUDES +=  .            \
					../../../device/lora/eml3047_lrwan    \
					../../../device/lora/sx1276   \
				    Lora/Crypto      \
					Lora/Phy         \
					Lora/Mac         \
					Lora/Core        \
					Lora/Mac/region  \
					Lora/Utilities   \
					Lora/hal
					# BSP/EML3047  \
					# BSP/sx1276       \

$(NAME)_INCLUDES := \
../../../board/eml3047/inc \
../../../platform/mcu/stm32l0xx/Drivers/STM32L0xx_HAL_Driver/Inc \
../../../platform/mcu/stm32l0xx/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy \
../../../platform/mcu/stm32l0xx/Drivers/CMSIS/Device/ST/STM32L0xx/Include \
../../../platform/mcu/stm32l0xx/Drivers/CMSIS/Include

$(NAME)_DEFINES := \
USE_HAL_DRIVER \
STM32L071xx
