HOST_OPENOCD := RV32M1
NAME := rv32m1impl

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/risc-v/riscy
$(NAME)_COMPONENTS += rhino libc cli rhino.vfs

GLOBAL_CFLAGS += -DCPU_RV32M1_ri5cy
GLOBAL_CFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -std=gnu99
GLOBAL_CFLAGS += -march=rv32imcxpulpv2 -MMD -MP
GLOBAL_CFLAGS += -Wno-format -Wno-incompatible-pointer-types -fno-store-merging

GLOBAL_ASMFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -std=gnu99
GLOBAL_ASMFLAGS += -march=rv32imcxpulpv2

GLOBAL_INCLUDES += ../../arch/risc-v/riscy/gcc \
                   ./aos

GLOBAL_LDFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -std=gnu99
GLOBAL_LDFLAGS += -Xlinker --gc-sections -Xlinker -static -Xlinker -z -Xlinker muldefs --specs=nosys.specs -nostartfiles
GLOBAL_LDFLAGS += -T platform/mcu/rv32m1/gcc/RV32M1_ri5cy_flash.ld

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

$(NAME)_SOURCES     += ./drivers/fsl_clock.c
$(NAME)_SOURCES     += ./drivers/fsl_common.c
$(NAME)_SOURCES     += ./drivers/fsl_gpio.c
$(NAME)_SOURCES     += ./drivers/fsl_lpuart.c
$(NAME)_SOURCES     += ./drivers/fsl_msmc.c
$(NAME)_SOURCES     += ./system_RV32M1_ri5cy.c
$(NAME)_SOURCES     += ./gcc/startup_RV32M1_ri5cy.S
$(NAME)_SOURCES     += ./hal/hal_uart.c
$(NAME)_SOURCES     += ./aos/aos.c
$(NAME)_SOURCES     += ./aos/soc_impl.c
