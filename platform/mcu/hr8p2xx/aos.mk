NAME := mcu_hr8p2xx
HOST_OPENOCD := hr8p2xx

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu hr8p2xx

$(NAME)_COMPONENTS += arch_armv6m rhino

GLOBAL_DEFINES += __PRINTF_USE_UART0__

GLOBAL_INCLUDES += drivers/        \
                   drivers/library/Include \
                   drivers/cmsis

$(NAME)_SOURCES += hal/hw.c \
	           hal/hal_uart.c 

# The library is built by Keil4 (source file is under driver directory)
ifeq ($(HOST_MCU_NAME), HR8P296)
$(NAME)_PREBUILT_LIBRARY := drivers/HR8P296.a
GLOBAL_DEFINES += HR8P296
endif

ifeq ($(HOST_MCU_NAME), HR8P287)
$(NAME)_PREBUILT_LIBRARY := drivers/HR8P287.a
GLOBAL_DEFINES += HR8P287
endif

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M0 --apcs=interwork -D__MICROLIB -g -O0 --split_sections
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M0 -g --apcs=interwork --pd "__MICROLIB SETA 1"
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M0                       \
                  -L --library_type=microlib               \
                  -L --strict -L --summary_stderr -L --map \
                  -L --xref -L --callgraph -L --symbols    \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
endif
