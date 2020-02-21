HOST_OPENOCD := MIMXRT1052
NAME         := mcu_mimxrt1052impl

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := driver & sdk for platform/mcu mimxrt1052impl

$(NAME)_COMPONENTS += arch_armv7m
$(NAME)_COMPONENTS += rhino newlib_stub osal_aos

GLOBAL_CFLAGS += -DA_LITTLE_ENDIAN
GLOBAL_CFLAGS += -DCPU_MIMXRT1052DVL6B

ifeq ($(COMPILER), armcc)
GLOBAL_CFLAGS += --c99 -c --cpu Cortex-M7.fp.dp -D__MICROLIB -g -O0 --apcs=interwork --split_sections
GLOBAL_ASMFLAGS += --cpu Cortex-M7.fp.dp -g --apcs=interwork --pd "__MICROLIB SETA 1"
GLOBAL_LDFLAGS += --cpu Cortex-M7.fp.dp
else
GLOBAL_CFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
GLOBAL_CFLAGS += -mcpu=cortex-m7 -mfloat-abi=hard -MMD -MP -mfpu=fpv5-d16
GLOBAL_CFLAGS += -Wno-format -Wno-incompatible-pointer-types

GLOBAL_ASMFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
GLOBAL_ASMFLAGS += -D__STARTUP_CLEAR_BSS
GLOBAL_ASMFLAGS += -D__STARTUP_INITIALIZE_NONCACHEDATA
GLOBAL_ASMFLAGS += -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16

GLOBAL_LDFLAGS += --specs=nano.specs --specs=nosys.specs
GLOBAL_LDFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
GLOBAL_LDFLAGS += -Xlinker --gc-sections -Xlinker -static -Xlinker -z -Xlinker muldefs
GLOBAL_LDFLAGS += -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS += -Wno-unused-value -Wno-strict-aliasing
endif

$(NAME)_SOURCES := drivers/fsl_clock.c  \
                   drivers/fsl_common.c \
                   drivers/fsl_gpio.c   \
                   drivers/fsl_lpuart.c \
                   system_MIMXRT1052.c  \
                   hal/hal_uart.c       \
                   hal/hal_flash.c      \
                   aos/aos.c            \
                   aos/soc_impl.c 

GLOBAL_INCLUDES += ./      \
                   drivers \
                   CMSIS/Include

# Component related source
$(NAME)_SOURCES += component/mini_bl/fsl_mini_bl.c
GLOBAL_INCLUDES += component/mini_bl

$(NAME)_SOURCES += boot/fsl_flexspi_nor_boot.c
GLOBAL_INCLUDES += boot
