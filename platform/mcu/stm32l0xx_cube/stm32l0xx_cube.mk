NAME := stm32l0xx_cube

HOST_OPENOCD := stm32l0xx

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv6m

$(NAME)_COMPONENTS += rhino libc

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192

GLOBAL_INCLUDES += ../../../arch/arm/armv6m/gcc/m0plus
GLOBAL_INCLUDES += \
                   Drivers/STM32L0xx_HAL_Driver/Inc \
                   Drivers/STM32L0xx_HAL_Driver/Inc/Legacy \
                   Drivers/CMSIS/Include \
                   Drivers/CMSIS/Device/ST/STM32L0xx/Include
 

$(NAME)_SOURCES := \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart_ex.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_dma.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim_ex.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c_ex.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ramfunc.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc_ex.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr_ex.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ex.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c  \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_adc.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_spi.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_usart.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_gpio.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_rtc.c \
Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_ll_rcc.c \
Drivers/CMSIS/Device/ST/STM32L0xx/Source/Templates/system_stm32l0xx.c

$(NAME)_SOURCES += aos/soc_impl.c \
                   aos/aos.c \
                   hal/hal_uart_stm32l0.c \
                   hal/hw.c 

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu Cortex-M0+ -D__MICROLIB -g --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS +=  --cpu=Cortex-M0+  
else
GLOBAL_CFLAGS += -mcpu=cortex-m0plus \
                 -march=armv6-m \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian \
				 -ffunction-sections  -specs=nosys.specs
GLOBAL_CFLAGS += -w 
GLOBAL_CFLAGS  += -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft
endif
					 
ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M0+ --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS +=  --cpu Cortex-M0+ \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m0plus \
                   -mlittle-endian \
                   -mthumb \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M0+ \
                  -L --strict \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M0+ 
else
LDFLAGS = -specs=nano.specs  -lc -lm -lnosys -Wl,--gc-sections

GLOBAL_LDFLAGS += -mcpu=cortex-m0plus  \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian \
                  -nostartfiles \
				  -specs=nosys.specs \

GLOBAL_LDFLAGS += $(LDFLAGS)
endif

