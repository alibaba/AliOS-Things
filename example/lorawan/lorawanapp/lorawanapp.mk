NAME := lorawanapp

GLOBAL_INCLUDES +=  .            

GLOBAL_DEFINES          += USE_FULL_LL_DRIVER
GLOBAL_DEFINES          += USE_B_EML3047
GLOBAL_DEFINES          += REGION_CN470


$(NAME)_SOURCES := lorawanapp.c              \
				     

$(NAME)_COMPONENTS := protocols.lorawan  \


$(NAME)_INCLUDES := \
../../../platform/mcu/stm32l0xx/Drivers/STM32L0xx_HAL_Driver/Inc \
../../../platform/mcu/stm32l0xx/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy \
../../../platform/mcu/stm32l0xx/Drivers/CMSIS/Device/ST/STM32L0xx/Include \
../../../platform/mcu/stm32l0xx/Drivers/CMSIS/Include

$(NAME)_DEFINES := \
USE_HAL_DRIVER \
STM32L071xx
		
