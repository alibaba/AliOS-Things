NAME := linkwan

GLOBAL_INCLUDES +=  .            

GLOBAL_DEFINES          += USE_FULL_LL_DRIVER
GLOBAL_DEFINES          += USE_B_EML3047
GLOBAL_DEFINES          += REGION_CN470A

linkwan := 1
ifeq ($(linkwantest), 1)
GLOBAL_DEFINES += LOW_POWER_DISABLE
endif

$(NAME)_SOURCES := linkwan.c              \

$(NAME)_COMPONENTS := protocols.lorawan  \


$(NAME)_INCLUDES := \
../../../platform/mcu/stm32l0xx/Drivers/STM32L0xx_HAL_Driver/Inc \
../../../platform/mcu/stm32l0xx/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy \
../../../platform/mcu/stm32l0xx/Drivers/CMSIS/Device/ST/STM32L0xx/Include \
../../../platform/mcu/stm32l0xx/Drivers/CMSIS/Include

$(NAME)_DEFINES := \
USE_HAL_DRIVER \
STM32L071xx
		
