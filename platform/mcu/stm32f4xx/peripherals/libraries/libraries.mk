
GLOBAL_INCLUDES += peripherals/libraries/                               \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/inc \
                   ../$(HOST_ARCH)/CMSIS

$(NAME)_SOURCES += peripherals/libraries/system_stm32f4xx.c                                \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/misc.c             \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c    \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_can.c    \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_crc.c    \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c    \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c    \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c   \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c  \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c   \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rng.c    \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c    \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_iwdg.c   \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c    \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c    \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c    \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c   \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c    \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c    \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c  \
                   peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_wwdg.c

# Add FSMC to non-F411/401 MCUs
ifeq ($(filter $(HOST_MCU_VARIANT), STM32F411 STM32F401),)
$(NAME)_SOURCES += peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fsmc.c
endif

# Add QSPI to F412 MCU
ifneq ($(filter $(HOST_MCU_VARIANT), STM32F412),)
$(NAME)_SOURCES += peripherals/libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_qspi.c
endif
