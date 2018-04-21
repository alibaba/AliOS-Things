NAME := stm32l4xx
HOST_OPENOCD := stm32l433
$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += libc rhino hal vfs digest_algorithm

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192

GLOBAL_INCLUDES += \
                   Drivers/STM32L4xx_HAL_Driver/Inc \
                   Drivers/STM32L4xx_HAL_Driver/Inc/Legacy \
                   Drivers/CMSIS/Include \
                   Drivers/CMSIS/Device/ST/STM32L4xx/Include \
                   src/$(HOST_MCU_NAME)/runapp \
                   src/$(HOST_MCU_NAME)/hal \
                   src/$(HOST_MCU_NAME)
                   
$(NAME)_SOURCES := Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c  \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc.c  \
		   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc_ex.c  \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c  \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_qspi.c   \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rng.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c \
		   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sai.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c   \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c  \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c   \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_sd.c \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_nor.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_nand.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_sdmmc.c    \
                   Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_fmc.c

$(NAME)_SOURCES += aos/soc_impl.c \
                   aos/trace_impl.c
                   
ifeq ($(HOST_MCU_NAME), STM32L433RC-Nucleo)
GLOBAL_CFLAGS += -DSTM32L433xx
$(NAME)_SOURCES += src/$(HOST_MCU_NAME)/hal/hal_gpio_stm32l4.c \
                   src/$(HOST_MCU_NAME)/hal/hal_i2c_stm32l4.c 
else ifeq ($(HOST_MCU_NAME), STM32L432KC-Nucleo)
GLOBAL_CFLAGS += -DSTM32L432xx 
endif
              
ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += src/STM32L433RC-Nucleo/startup_stm32l433xx_keil.s
     
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += src/STM32L433RC-Nucleo/startup_stm32l433xx_iar.s

else
$(NAME)_SOURCES += src/STM32L433RC-Nucleo/startup_stm32l433xx.s

endif
     
$(NAME)_SOURCES += src/$(HOST_MCU_NAME)/runapp/soc_init.c \
                   src/$(HOST_MCU_NAME)/runapp/stm32l4xx_hal_msp.c \
                   src/$(HOST_MCU_NAME)/runapp/stm32l4xx_it.c \
                   src/$(HOST_MCU_NAME)/runapp/system_stm32l4xx.c \
                   src/$(HOST_MCU_NAME)/runapp/aos.c  \
                   src/$(HOST_MCU_NAME)/hal/hal_uart_stm32l4.c \
                   src/$(HOST_MCU_NAME)/hal/hw.c \
                   src/$(HOST_MCU_NAME)/hal/flash_l4.c \
                   src/$(HOST_MCU_NAME)/hal/flash_port.c

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M4 --apcs=/hardfp --fpu=vfpv4_sp_d16 -D__MICROLIB -g --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M4 \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m4 \
                 -march=armv7-m  \
                 -mlittle-endian \
                 -mthumb -mthumb-interwork \
                 -mfloat-abi=hard \
                 -mfpu=fpv4-sp-d16 \
                 -w
GLOBAL_CFLAGS  += -D__VFP_FP__
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M4 -g --apcs=/hardfp --fpu=vfpv4_sp_d16 --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M4 \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m4 \
                   -mlittle-endian \
                   -mthumb \
                   -mfloat-abi=hard \
                   -mfpu=fpv4-sp-d16 \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M4   \
                  -L --fpu=vfpv4_sp_d16 \
                  -L --apcs=/hardfp \
                  -L --strict \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M4.vfp

else
GLOBAL_LDFLAGS += -mcpu=cortex-m4  \
                  -mlittle-endian  \
                  -mthumb -mthumb-interwork \
                  --specs=nosys.specs \
                  -mfloat-abi=hard \
                  -mfpu=fpv4-sp-d16 \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/mcu/stm32l4xx/src/STM32L433RC-Nucleo/STM32L433.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/mcu/stm32l4xx/src/STM32L433RC-Nucleo/STM32L433.icf
else
GLOBAL_LDFLAGS += -T platform/mcu/stm32l4xx/src/STM32L433RC-Nucleo/STM32L433.ld
endif


ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := src/STM32L433RC-Nucleo/startup_stm32l433xx_keil.o
$(NAME)_LINK_FILES += src/$(HOST_MCU_NAME)/runapp/stm32l4xx_it.o
$(NAME)_LINK_FILES += src/$(HOST_MCU_NAME)/runapp/stm32l4xx_hal_msp.o
endif
