NAME := stm32f7xx
HOST_OPENOCD := stm32f769
$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += libc rhino hal vfs digest_algorithm

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192

RELATIVE_PATH_MCU = ../..

GLOBAL_INCLUDES += ../../$(RELATIVE_PATH_MCU)/arch/arm/armv7m/gcc/m7

GLOBAL_INCLUDES += \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Inc \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy \
                   $(RELATIVE_PATH_MCU)/Drivers/CMSIS/Include \
                   $(RELATIVE_PATH_MCU)/Drivers/CMSIS/Device/ST/STM32F7xx/Include \
                   $(RELATIVE_PATH_MCU)/src/STM32F769I-Discovery/rhinorun \
                   $(RELATIVE_PATH_MCU)/src/STM32F769I-Discovery \
				   $(RELATIVE_PATH_MCU)/hal
                   
$(NAME)_SOURCES := $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c  \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c  \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash_ex.c \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c.c    \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c_ex.c \
				   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c   \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c   \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c    \
				   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c    \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c \
				   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c    \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c \
				   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_tim.c    \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_pwr.c \
                   $(RELATIVE_PATH_MCU)/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c  \
				   $(RELATIVE_PATH_MCU)/hal/cache.c  \
				   $(RELATIVE_PATH_MCU)/hal/hw.c  \
				   $(RELATIVE_PATH_MCU)/hal/hal_uart_stm32f7.c
				   

$(NAME)_SOURCES += $(RELATIVE_PATH_MCU)/aos/soc_impl.c \
                   $(RELATIVE_PATH_MCU)/aos/trace_impl.c
                   

GLOBAL_CFLAGS += -DSTM32F769xx
                   
ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += $(RELATIVE_PATH_MCU)/src/STM32F769I-Discovery/startup_stm32f769xx_keil.s
     
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += $(RELATIVE_PATH_MCU)/src/STM32F769I-Discovery/startup_stm32f769xx_iar.s

else
$(NAME)_SOURCES += $(RELATIVE_PATH_MCU)/src/STM32F769I-Discovery/startup_stm32f769xx.s

endif
     
ifeq ($(HOST_MCU_NAME), STM32F769I-Discovery)
$(NAME)_SOURCES += $(RELATIVE_PATH_MCU)/src/STM32F769I-Discovery/rhinorun/soc_init.c \
                   $(RELATIVE_PATH_MCU)/src/STM32F769I-Discovery/rhinorun/stm32f7xx_hal_msp.c \
                   $(RELATIVE_PATH_MCU)/src/STM32F769I-Discovery/rhinorun/stm32f7xx_it.c \
                   $(RELATIVE_PATH_MCU)/src/STM32F769I-Discovery/rhinorun/system_stm32f7xx.c
endif

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=7E-M -D__MICROLIB -g --apcs=interwork --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M7 \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m7 \
                 -march=armv7-m  \
                 -mlittle-endian \
                 -mthumb -mthumb-interwork \
                 -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=7E-M -g --apcs=interwork --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M7 \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m7 \
                   -march=armv7-m  \
                   -mlittle-endian \
                   -mthumb -mthumb-interwork \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=7E-M   \
                  -L --strict \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M7.vfp

else
GLOBAL_LDFLAGS += -mcpu=cortex-m7  \
                  -mlittle-endian  \
                  -mthumb -mthumb-interwork \
                  --specs=nosys.specs \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/mcu/stm32f7xx/src/STM32F769I-Discovery/STM32F769I_DISCOVERY.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/mcu/stm32f7xx/src/STM32F769I-Discovery/stm32f769xx.icf
else
GLOBAL_LDFLAGS += -T platform/mcu/stm32f7xx/src/STM32F769I-Discovery/STM32F769NIHx_FLASH.ld
endif


ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := $(RELATIVE_PATH_MCU)/src/STM32F769I-Discovery/startup_stm32f769xx_keil.o
$(NAME)_LINK_FILES += $(RELATIVE_PATH_MCU)/src/STM32F769I-Discovery/rhinorun/stm32f7xx_it.o
$(NAME)_LINK_FILES += $(RELATIVE_PATH_MCU)/src/STM32F769I-Discovery/rhinorun/stm32f7xx_hal_msp.o
endif

