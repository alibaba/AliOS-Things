HOST_OPENOCD := MKL81Z7
NAME         := mcu_mkl81z7impl

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu mkl81z7impl

$(NAME)_COMPONENTS += arch_armv6m
$(NAME)_COMPONENTS += rhino newlib_stub

GLOBAL_CFLAGS += -DCPU_MKL81Z128VLK7
GLOBAL_CFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
GLOBAL_CFLAGS += -mcpu=cortex-m0plus -mfloat-abi=soft -MMD -MP
GLOBAL_CFLAGS += -Wno-format -Wno-incompatible-pointer-types

GLOBAL_ASMFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
GLOBAL_ASMFLAGS += -D__STARTUP_CLEAR_BSS
GLOBAL_ASMFLAGS += -mcpu=cortex-m0plus -mfloat-abi=soft
GLOBAL_INCLUDES += ./ \
                   CMSIS/Include \
                   drivers

GLOBAL_LDFLAGS += --specs=nano.specs --specs=nosys.specs
GLOBAL_LDFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
GLOBAL_LDFLAGS += -Xlinker --gc-sections -Xlinker -static -Xlinker -z -Xlinker muldefs
GLOBAL_LDFLAGS += -mcpu=cortex-m0plus -mfloat-abi=soft
GLOBAL_LDFLAGS += -T platform/mcu/mkl81z7/gcc/MKL81Z128xxx7_flash.ld

$(NAME)_SOURCES += ./drivers/fsl_clock.c
$(NAME)_SOURCES += ./drivers/fsl_common.c
$(NAME)_SOURCES += ./drivers/fsl_flash.c
$(NAME)_SOURCES += ./drivers/fsl_gpio.c
$(NAME)_SOURCES += ./drivers/fsl_lpuart.c
$(NAME)_SOURCES += ./drivers/fsl_smc.c
$(NAME)_SOURCES += ./system_MKL81Z7.c
$(NAME)_SOURCES += ./gcc/startup_MKL81Z7.S
$(NAME)_SOURCES += ./hal/hal_flash.c
$(NAME)_SOURCES += ./hal/hal_uart.c
$(NAME)_SOURCES += ./aos/aos.c
$(NAME)_SOURCES += ./aos/soc_impl.c

