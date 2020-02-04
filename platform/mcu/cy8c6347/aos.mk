NAME := mcu_cy8c6347

HOST_OPENOCD := cy8c6347

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := driver & sdk for platform/mcu cy8c6347

$(NAME)_COMPONENTS += arch_armv7m
$(NAME)_COMPONENTS += newlib_stub rhino

GLOBAL_INCLUDES += ../../arch/arm/armv7m/gcc/m4

GLOBAL_INCLUDES += drivers                                  \
                   drivers/PSoC6                            \
                   drivers/PSoC6/pdl/drivers/peripheral     \
                   drivers/PSoC6/pdl/cmsis/include                \
                   drivers/PSoC6/pdl/devices/psoc6/ip             \
                   drivers/PSoC6/pdl/devices/psoc6/psoc63/include \
                   drivers/PSoC6/pdl/middleware

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS += --c99 --cpu=7E-M -D__MICROLIB -g --apcs=interwork --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M4  \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m4           \
                 -mlittle-endian           \
                 -mthumb -mthumb-interwork \
                 -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=7E-M -g --apcs=interwork --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M4  \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m4           \
                   -mlittle-endian           \
                   -mthumb -mthumb-interwork \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=7E-M                         \
                  -L --strict                           \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M4.vfp

else
GLOBAL_LDFLAGS += -mcpu=cortex-m4           \
                  -mlittle-endian           \
                  -mthumb -mthumb-interwork \
                  -nostartfiles             \
                  --specs=nosys.specs       \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/mcu/cy8c6347/drivers/linker_file/cy8c6xx7_cm4_dual.scat
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/mcu/cy8c6347/drivers/linker_file/cy8c6xx7_cm4_dual.icf
else
GLOBAL_LDFLAGS += -T platform/mcu/cy8c6347/drivers/linker_file/cy8c6xx7_cm4_dual.ld
endif

$(NAME)_SOURCES := drivers/cy_ipc_config.c                                         \
                   drivers/system_psoc63_cm4.c                                     \
				   drivers/startup/startup_psoc63_cm4.s                            \
                   drivers/PSoC6/cyfitter_sysint_cfg.c                             \
                   drivers/PSoC6/cymetadata.c                                        \
                   drivers/PSoC6/UART1.c                                           \
                   drivers/PSoC6/UART5.c                                           \
                   drivers/PSoC6/pdl/drivers/peripheral/flash/cy_flash.c           \
                   drivers/PSoC6/pdl/drivers/peripheral/gpio/cy_gpio.c             \
                   drivers/PSoC6/pdl/drivers/peripheral/ipc/cy_ipc_drv.c           \
                   drivers/PSoC6/pdl/drivers/peripheral/ipc/cy_ipc_pipe.c          \
                   drivers/PSoC6/pdl/drivers/peripheral/ipc/cy_ipc_sema.c          \
                   drivers/PSoC6/pdl/drivers/peripheral/scb/cy_scb_common.c        \
                   drivers/PSoC6/pdl/drivers/peripheral/scb/cy_scb_ezi2c.c         \
                   drivers/PSoC6/pdl/drivers/peripheral/scb/cy_scb_i2c.c           \
                   drivers/PSoC6/pdl/drivers/peripheral/scb/cy_scb_spi.c           \
                   drivers/PSoC6/pdl/drivers/peripheral/scb/cy_scb_uart.c          \
                   drivers/PSoC6/pdl/drivers/peripheral/sysclk/cy_sysclk.c         \
                   drivers/PSoC6/pdl/drivers/peripheral/sysint/cy_sysint.c         \
                   drivers/PSoC6/pdl/drivers/peripheral/syslib/cy_syslib.c         \
                   drivers/PSoC6/pdl/drivers/peripheral/syslib/gcc/cy_syslib_gcc.S \
                   drivers/PSoC6/pdl/drivers/peripheral/syspm/cy_syspm.c           \
                   drivers/PSoC6/pdl/drivers/peripheral/systick/cy_systick.c       \
                   drivers/PSoC6/pdl/drivers/peripheral/wdt/cy_wdt.c               \
                   aos/soc_impl.c                                                  \
                   hal/hw.c                                                        \
                   hal/flash_port.c                                                \
                   hal/uart_port.c                                                 \
                   runapp/aos.c
