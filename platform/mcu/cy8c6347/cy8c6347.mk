
NAME := cy8c6347

HOST_OPENOCD := cy8c6347

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += libc rhino rhino.fs.kv

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192
GLOBAL_DEFINES += CY8C6347BZI_BLD53

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=7E-M -D__MICROLIB -g -o0 --apcs=interwork --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M4 \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m4 \
                 -march=armv7-m  \
                 -mlittle-endian \
                 -mthumb -mthumb-interwork \
                 -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=7E-M -g --apcs=interwork --library_type=microlib --pd "__MICROLIB SETA 1"
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

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=7E-M -L --strict -L --xref -L --callgraph -L --symbols -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M4.vfp
else
GLOBAL_LDFLAGS += -mcpu=cortex-m4 -mlittle-endian -mthumb -mthumb-interwork -nostartfiles --specs=nosys.specs $(CLIB_LDFLAGS_NANO_FLOAT)
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/mcu/cy8c6347/Generated_Source/linker_file/mdk/cy8c6xx7_cm4_dual.scat
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/mcu/cy8c6347/Generated_Source/linker_file/iar/cy8c6xx7_cm4_dual.icf
else
GLOBAL_LDFLAGS += -T platform/mcu/cy8c6347/Generated_Source/linker_file/armgcc/cy8c6xx7_cm4_dual.ld
endif

GLOBAL_INCLUDES += ../../arch/arm/armv7m/gcc/m4
GLOBAL_INCLUDES += ./hal
GLOBAL_INCLUDES += ./Generated_Source
GLOBAL_INCLUDES += ./Generated_Source/PSoC6
GLOBAL_INCLUDES += ./Generated_Source/PSoC6/pdl
GLOBAL_INCLUDES += ./Generated_Source/PSoC6/pdl/cmsis/include
GLOBAL_INCLUDES += ./Generated_Source/PSoC6/pdl/devices/psoc6/ip
GLOBAL_INCLUDES += ./Generated_Source/PSoC6/pdl/devices/psoc6/psoc63/include
GLOBAL_INCLUDES += ./Generated_Source/PSoC6/pdl/drivers/peripheral
GLOBAL_INCLUDES += ./Generated_Source/PSoC6/pdl/middleware

$(NAME)_SOURCES += ./Generated_Source/psoc63_m0plus_image.c
$(NAME)_SOURCES += ./Generated_Source/main_cm4.c
$(NAME)_SOURCES += ./Generated_Source/system_psoc63_cm4.c
$(NAME)_SOURCES += ./Generated_Source/cy_ipc_config.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/cyfitter_sysint_cfg.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/cymetadata.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/flash/cy_flash.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/gpio/cy_gpio.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/ipc/cy_ipc_drv.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/ipc/cy_ipc_pipe.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/ipc/cy_ipc_sema.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/prot/cy_prot.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/scb/cy_scb_common.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/scb/cy_scb_ezi2c.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/scb/cy_scb_i2c.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/scb/cy_scb_spi.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/scb/cy_scb_uart.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/sysclk/cy_sysclk.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/sysint/cy_sysint.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/syslib/cy_syslib.c
ifeq ($(COMPILER),armcc)
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/syslib/mdk/cy_syslib_mdk.s
else ifeq ($(COMPILER),iar)
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/syslib/iar/cy_syslib_iar.s
else
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/syslib/gcc/cy_syslib_gcc.s
endif
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/syspm/cy_syspm.c
$(NAME)_SOURCES += ./Generated_Source/PSoC6/pdl/drivers/peripheral/systick/cy_systick.c
ifeq ($(COMPILER),armcc)
$(NAME)_SOURCES += ./Generated_Source/startup/mdk/startup_psoc63_cm4.s
else ifeq ($(COMPILER),iar)
$(NAME)_SOURCES += ./Generated_Source/startup/iar/startup_psoc63_cm4.s
else
$(NAME)_SOURCES += ./Generated_Source/startup/armgcc/startup_psoc63_cm4.S
endif
$(NAME)_SOURCES += ./hal/flash_port.c
$(NAME)_SOURCES += ./hal/ota_port.c
$(NAME)_SOURCES += ./hal/uart_port.c
$(NAME)_SOURCES += ./hal/hw.c
$(NAME)_SOURCES += ./aos/soc_impl.c
$(NAME)_SOURCES += ./aos/trace_impl.c
$(NAME)_SOURCES += ./runapp/aos.c
