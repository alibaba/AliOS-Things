NAME = gd32f4xx

# Host architecture is ARM Cortex M4
HOST_ARCH := Cortex-M4

# Host MCU alias for OpenOCD
HOST_OPENOCD := gd32f4xx

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += libc rhino hal

# Global defines
GLOBAL_DEFINES := USE_STDPERIPH_DRIVER
GLOBAL_DEFINES += GD32F450

# Global includes
GLOBAL_INCLUDES += ../../arch/arm/armv7m/gcc/m4
GLOBAL_INCLUDES += hal \
                   src/helloworld \
                   driver/Firmware/CMSIS \
                   driver/Firmware/CMSIS/GD/GD32F4xx/Include \
                   driver/Firmware/GD32F4xx_standard_peripheral/Include \
                   driver/Utilities
                   


GLOBAL_CFLAGS += -DGD32F4xx 

GLOBAL_CFLAGS += -mcpu=cortex-m4 \
                 -march=armv7-m \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w


ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=7E-M -g --apcs=interwork -D__MICROLIB -DGD32F4xx
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M4 \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m4 \
                   -march=armv7-m  \
                   -mlittle-endian \
                   -mthumb -mthumb-interwork \
                   -w
endif


GLOBAL_LDFLAGS += -mcpu=cortex-m4        \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian \
                  -nostartfiles \
                  --specs=nosys.specs \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing


$(NAME)_SOURCES := src/helloworld/main.c \
                   src/helloworld/gd32f4xx_it.c \
                   src/helloworld/soc_init.c \
                   aos/soc_impl.c \
                   aos/trace_impl.c \
                   hal/flash_port.c \
                   hal/ringbuf.c \
                   hal/uart.c  \
                   driver/Firmware/CMSIS/GD/GD32F4xx/Source/system_gd32f4xx.c \
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_adc.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_can.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_crc.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_ctc.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_dac.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_dbg.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_dma.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_exmc.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_exti.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_fmc.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_fwdgt.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_gpio.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_i2c.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_misc.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_pmu.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_rcu.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_rtc.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_sdio.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_spi.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_timer.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_usart.c\
                   driver/Firmware/GD32F4xx_standard_peripheral/Source/gd32f4xx_wwdgt.c\
                   driver/Utilities/gd32f450z_eval.c



GLOBAL_LDFLAGS  += -T platform/mcu/gd32f4xx/gd32_flash.ld


GLOBAL_DEFINES += CONFIG_ARM
