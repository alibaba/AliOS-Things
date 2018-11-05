NAME := atsame54

# Host architecture is ARM Cortex M4
HOST_ARCH := Cortex-M4

# Host MCU alias for OpenOCD
HOST_OPENOCD := atsame54

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += libc rhino hal modules.fs.kv vfs digest_algorithm

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192

GLOBAL_INCLUDES += \
                   aos \
                   Drivers \
                   Drivers/config \
                   Drivers/hal/include \
                   Drivers/hal/utils/include \
                   Drivers/hri \
                   Drivers/CMSIS/Include \
                   Drivers/include \
                   Drivers/sd_mmc   \
                   src/ATSAME54-XPRO/runapp

$(NAME)_SOURCES := Drivers/atmel_start.c  \
                   Drivers/driver_init.c  \
                   Drivers/sd_mmc_start.c   \
                   Drivers/sd_mmc/sd_mmc.c  \
                   Drivers/hal/src/hal_atomic.c \
                   Drivers/hal/src/hal_delay.c \
                   Drivers/hal/src/hal_flash.c \
                   Drivers/hal/src/hal_gpio.c    \
                   Drivers/hal/src/hal_init.c \
                   Drivers/hal/src/hal_io.c    \
                   Drivers/hal/src/hal_ext_irq.c \
                   Drivers/hal/src/hal_rtos.c \
                   Drivers/hal/src/hal_sleep.c   \
                   Drivers/hal/src/hal_usart_os.c \
                   Drivers/hal/src/hal_ext_irq.c    \
                   Drivers/hal/utils/src/utils_assert.c    \
                   Drivers/hal/utils/src/utils_event.c \
                   Drivers/hal/utils/src/utils_list.c    \
                   Drivers/hal/utils/src/utils_ringbuffer.c \
                   Drivers/hal/utils/src/utils_syscalls.c    \
                   Drivers/hpl/core/hpl_core_m4.c    \
                   Drivers/hpl/core/hpl_init.c   \
                   Drivers/hpl/dmac/hpl_dmac.c  \
                   Drivers/hpl/eic/hpl_eic.c \
                   Drivers/hpl/gclk/hpl_gclk.c   \
                   Drivers/hpl/mclk/hpl_mclk.c    \
                   Drivers/hpl/nvmctrl/hpl_nvmctrl.c    \
                   Drivers/hpl/osc32kctrl/hpl_osc32kctrl.c \
                   Drivers/hpl/oscctrl/hpl_oscctrl.c \
                   Drivers/hpl/pm/hpl_pm.c \
                   Drivers/hpl/ramecc/hpl_ramecc.c    \
                   Drivers/hpl/sercom/hpl_sercom.c    \
                   Drivers/hpl/systick/hpl_systick.c    \
                   Drivers/hpl/sdhc/hpl_sdhc.c  \
                   Drivers/hpl/eic/hpl_eic.c    \
                   Drivers/hal/src/hal_mci_sync.c   \
                   hal/hal_flash_port.c        \
                   hal/hal_timer_port.c        \
                   hal/hal_uart_start.c        \
                   hal/ota_port.c              \
                   hal/misc.c


$(NAME)_SOURCES += aos/aos.c \
                   aos/soc_impl.c \
                   aos/trace_impl.c

GLOBAL_CFLAGS += -D__SAME54P20A__

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += Drivers/armcc/system_same54.c \
                   Drivers/armcc/arm/startup_same54.s
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += Drivers/iar/system_same54.c \
                   Drivers/iar/iar/startup_same54.c
else
$(NAME)_SOURCES += Drivers/gcc/system_same54.c \
                   Drivers/gcc/gcc/startup_same54.c
endif

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=7E-M -D__MICROLIB -g --apcs=interwork --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M4 \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m4 \
                 -march=armv7-m \
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
GLOBAL_LDFLAGS += -L --cpu=7E-M   \
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
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/mcu/atsamd5x_e5x/Drivers/armcc/armcc/x.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/mcu/atsamd5x_e5x/Drivers/iar/iar/same54p20a_flash.icf
else
GLOBAL_LDFLAGS += -T platform/mcu/atsamd5x_e5x/Drivers/gcc/gcc/same54p20a_flash.ld
endif

