HOST_OPENOCD := CY8C4147
NAME := cy8c4147

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv6m
$(NAME)_COMPONENTS += rhino libc modules.fs.kv

GLOBAL_CFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
GLOBAL_CFLAGS += -mcpu=cortex-m0plus -mfloat-abi=soft -MMD -MP
GLOBAL_CFLAGS += -Wno-format -Wno-incompatible-pointer-types

GLOBAL_ASMFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
GLOBAL_ASMFLAGS += -mcpu=cortex-m0plus -mfloat-abi=soft

GLOBAL_LDFLAGS += --specs=nano.specs --specs=nosys.specs
GLOBAL_LDFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
GLOBAL_LDFLAGS += -Xlinker --gc-sections -Xlinker -static -Xlinker -z -Xlinker muldefs
GLOBAL_LDFLAGS += -mcpu=cortex-m0plus -mfloat-abi=soft
GLOBAL_LDFLAGS += -T platform/mcu/cy8c4147/Generated_Source/PSoC4/cm0plusgcc.ld
GLOBAL_LDFLAGS += -L platform/mcu/cy8c4147/Generated_Source/PSoC4

GLOBAL_INCLUDES += .
GLOBAL_INCLUDES += Generated_Source/PSoC4
GLOBAL_INCLUDES += hal
GLOBAL_INCLUDES += ../../../include
GLOBAL_INCLUDES += ../../arch/arm/armv6m/gcc/m0plus/

$(NAME)_SOURCES     += ./Generated_Source/PSoC4/CyBootAsmGnu.s
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/Cm0plusStart.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/cy_em_eeprom.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/CyDMA.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/cyfitter_cfg.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/CyFlash.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/CyLFClk.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/CyLib.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/cymetadata.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/cyPm.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/cyutils.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/SCB3.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/SCB3_BOOT.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/SCB3_PM.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/SCB3_rx.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/SCB3_rx_PM.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/SCB3_SCB_IRQ.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/SCB3_SCBCLK.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/SCB3_SPI_UART.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/SCB3_SPI_UART_INT.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/SCB3_tx.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/SCB3_tx_PM.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/SCB3_UART.c
$(NAME)_SOURCES     += ./Generated_Source/PSoC4/SCB3_UART_BOOT.c
$(NAME)_SOURCES     += ./hal/flash_port.c
$(NAME)_SOURCES     += ./hal/hw.c
$(NAME)_SOURCES     += ./hal/uart_port.c
$(NAME)_SOURCES     += ./aos/soc_impl.c
$(NAME)_SOURCES     += ./aos/trace_impl.c
$(NAME)_SOURCES     += ./runapp/aos.c
