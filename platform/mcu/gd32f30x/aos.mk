NAME := mcu_gd32f30x
HOST_OPENOCD := gd32f30x

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu gd32f30x

$(NAME)_COMPONENTS += arch_armv7m rhino

GLOBAL_DEFINES += USE_STDPERIPH_DRIVER GD32F30x CONFIG_ARM

GLOBAL_INCLUDES += drivers/Firmware/CMSIS \
                   drivers/Firmware/CMSIS/GD/GD32F30x/Include \
                   drivers/Firmware/GD32F30x_standard_peripheral/Include \
                   drivers/Utilities

$(NAME)_SOURCES += hal/uart.c \
                   hal/flash.c \
                   drivers/Firmware/CMSIS/GD/GD32F30x/Source/system_gd32f30x.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_adc.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_bkp.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_can.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_crc.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_ctc.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_dac.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_dbg.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_dma.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_exmc.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_exti.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_fmc.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_fwdgt.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_gpio.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_i2c.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_misc.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_pmu.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_rcu.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_rtc.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_sdio.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_spi.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_timer.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_usart.c \
                   drivers/Firmware/GD32F30x_standard_peripheral/Source/gd32f30x_wwdgt.c \
                   drivers/Utilities/ringbuf.c \
                   drivers/Utilities/gd32f30x_fmc_ext.c

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_CFLAGS += -w -mcpu=cortex-m4 \
                 -march=armv7-m \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=7E-M -g --apcs=interwork -D__MICROLIB -DGD32F30x
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


