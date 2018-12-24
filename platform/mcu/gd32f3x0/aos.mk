NAME := mcu_gd32f3x0
HOST_OPENOCD := gd32f3x0

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu gd32f3x0

$(NAME)_COMPONENTS += arch_armv7m rhino

GLOBAL_DEFINES += USE_STDPERIPH_DRIVER GD32F3x0 CONFIG_ARM

GLOBAL_INCLUDES += drivers/Firmware/CMSIS \
                   drivers/Firmware/CMSIS/GD/GD32F3x0/Include \
                   drivers/Firmware/GD32F3x0_standard_peripheral/Include \
                   drivers/Utilities

$(NAME)_SOURCES += hal/uart.c \
                   hal/flash.c \
                   drivers/Firmware/CMSIS/GD/GD32F3x0/Source/system_gd32f3x0.c \
                   drivers/Firmware/GD32F3x0_standard_peripheral/Source/gd32f3x0_dma.c \
                   drivers/Firmware/GD32F3x0_standard_peripheral/Source/gd32f3x0_exti.c \
                   drivers/Firmware/GD32F3x0_standard_peripheral/Source/gd32f3x0_fmc.c \
                   drivers/Firmware/GD32F3x0_standard_peripheral/Source/gd32f3x0_gpio.c \
                   drivers/Firmware/GD32F3x0_standard_peripheral/Source/gd32f3x0_misc.c \
                   drivers/Firmware/GD32F3x0_standard_peripheral/Source/gd32f3x0_pmu.c \
                   drivers/Firmware/GD32F3x0_standard_peripheral/Source/gd32f3x0_rcu.c \
                   drivers/Firmware/GD32F3x0_standard_peripheral/Source/gd32f3x0_syscfg.c \
                   drivers/Firmware/GD32F3x0_standard_peripheral/Source/gd32f3x0_timer.c \
                   drivers/Firmware/GD32F3x0_standard_peripheral/Source/gd32f3x0_usart.c \
                   drivers/Firmware/GD32F3x0_standard_peripheral/Source/gd32f3x0_wwdgt.c \
                   drivers/Utilities/ringbuf.c

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_CFLAGS += -w -mcpu=cortex-m4 \
                 -march=armv7-m \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=7E-M -g --apcs=interwork -D__MICROLIB -DGD32F3x0
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


