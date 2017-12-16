#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME = STM32F4xx_Peripheral_Drivers

GLOBAL_INCLUDES := .

# Include STM32F2xx Standard Peripheral Libraries
$(NAME)_COMPONENTS += mcu/stm32f4xx/peripherals/libraries

$(NAME)_SOURCES := platform_adc.c \
                   platform_rtc.c \
                   platform_gpio.c \
                   platform_i2c.c \
                   platform_rng.c \
                   platform_mcu_powersave.c \
                   platform_pwm.c \
                   platform_flash.c \
                   platform_spi.c \
                   platform_uart.c \
                   platform_watchdog.c \
                   RingBufferUtils.c
                   
