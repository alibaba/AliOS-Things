NAME = STM32F4xx_Peripheral_Libraries

GLOBAL_INCLUDES :=  . \
                    STM32F4xx_StdPeriph_Driver/inc \
                    ../../../$(HOST_ARCH)/CMSIS

$(NAME)_SOURCES := \
                   system_stm32f4xx.c \
                   STM32F4xx_StdPeriph_Driver/src/misc.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_can.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_crc.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rng.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_iwdg.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c \
                   STM32F4xx_StdPeriph_Driver/src/stm32f4xx_wwdg.c

# Add FSMC to non-F411/401 MCUs
ifeq ($(filter $(HOST_MCU_VARIANT), STM32F411 STM32F401),)                
$(NAME)_SOURCES += STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fsmc.c
endif 

# Add QSPI to F412 MCU
ifneq ($(filter $(HOST_MCU_VARIANT), STM32F412),)                
$(NAME)_SOURCES += STM32F4xx_StdPeriph_Driver/src/stm32f4xx_qspi.c
endif 
