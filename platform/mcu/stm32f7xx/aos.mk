NAME         := mcu_stm32f7xx
HOST_OPENOCD := stm32f769

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu stm32f7xx

$(NAME)_COMPONENTS += arch_armv7m
$(NAME)_COMPONENTS += newlib_stub rhino

RELATIVE_PATH_MCU = .

GLOBAL_INCLUDES += ../../arch/arm/armv7m/gcc/m7

GLOBAL_INCLUDES += $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Inc          \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy   \
                   $(RELATIVE_PATH_MCU)/Drivers/CMSIS/Include                     \
                   $(RELATIVE_PATH_MCU)/Drivers/CMSIS/Device/ST/STM32F7xx/Include \
                   $(RELATIVE_PATH_MCU)/hal

$(NAME)_SOURCES := $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c          \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c    \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash_ex.c \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c.c      \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c_ex.c   \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c      \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c   \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c     \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c     \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c      \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c   \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c   \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c      \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c   \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_tim.c       \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_pwr.c       \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c   \
                   $(RELATIVE_PATH_MCU)/hal/hal_cache.c                                               \
                   $(RELATIVE_PATH_MCU)/hal/hw.c                                                  \
                   $(RELATIVE_PATH_MCU)/hal/hal_uart_stm32f7.c


GLOBAL_CFLAGS += -DSTM32F769xx


ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=7E-M -D__MICROLIB -g --apcs=interwork --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M7  \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m7           \
                 -mlittle-endian           \
                 -mthumb -mthumb-interwork \
                 -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=7E-M -g --apcs=interwork --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M7  \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m7           \
                   -mlittle-endian           \
                   -mthumb -mthumb-interwork \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=7E-M                         \
                  -L --strict                           \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M7.vfp

else
GLOBAL_LDFLAGS += -mcpu=cortex-m7           \
                  -mlittle-endian           \
                  -mthumb -mthumb-interwork \
                  --specs=nosys.specs       \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif
