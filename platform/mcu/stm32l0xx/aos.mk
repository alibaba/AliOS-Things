NAME := mcu_stm32l0xx

HOST_OPENOCD := stm32l0xx

SPI_WIFI_ENABLED := true

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := driver & sdk for platform/mcu stm32l0xx

$(NAME)_COMPONENTS += arch_armv6m
# $(NAME)_COMPONENTS += libc rhino hal netmgr mbedtls cjson cli
$(NAME)_COMPONENTS += rhino

GLOBAL_DEFINES += STM32_USE_SPI_WIFI

GLOBAL_INCLUDES += ../../arch/arm/armv6m/gcc/m0plus
GLOBAL_INCLUDES += Drivers/STM32L0xx_HAL_Driver/Inc          \
                   Drivers/STM32L0xx_HAL_Driver/Inc/Legacy   \
                   Drivers/CMSIS/Device/ST/STM32L0xx/Include \
                   Drivers/CMSIS/Include

GLOBAL_CFLAGS += -mcpu=cortex-m0plus       \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w

GLOBAL_LDFLAGS += -mcpu=cortex-m0plus       \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian           \
                  -nostartfiles             \
                  --specs=nosys.specs       \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS += -Wno-unused-value -Wno-strict-aliasing

$(NAME)_INCLUDES := Drivers/STM32L0xx_HAL_Driver/Inc          \
                    Drivers/STM32L0xx_HAL_Driver/Inc/Legacy   \
                    Drivers/CMSIS/Device/ST/STM32L0xx/Include \
                    Drivers/CMSIS/Include                     \
                    hal

$(NAME)_SOURCES := aos/soc_impl.c                                                 \
                   hal/flash.c                                                    \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart_ex.c       \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.c          \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_dma.c           \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim.c           \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim_ex.c        \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c_ex.c        \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c               \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rtc.c           \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rtc_ex.c        \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ramfunc.c \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc.c           \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc_ex.c        \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr_ex.c        \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash.c         \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ex.c      \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart.c          \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c.c           \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr.c           \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c        \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_adc.c            \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_spi.c            \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_usart.c          \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_gpio.c           \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_rtc.c            \
                   Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_rcc.c            \
                   hal/hal_timer_stm32l0.c

$(NAME)_DEFINES := USE_HAL_DRIVER

include $($(NAME)_LOCATION)/$(HOST_MCU_NAME).mk
