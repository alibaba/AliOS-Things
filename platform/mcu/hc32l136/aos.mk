NAME := mcu_hc32l136
HOST_OPENOCD := hc32l136

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu hc32l136

$(NAME)_COMPONENTS += arch_armv6m rhino

GLOBAL_DEFINES += USE_HAL_DRIVER

GLOBAL_INCLUDES += drivers/        \
                   drivers/cmsis

$(NAME)_SOURCES += hal/hw.c \
                   hal/uart.c \
                   drivers/system_hc32l136.c

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M0+ --apcs=interwork -D__MICROLIB -g --split_sections
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M0+ -g --apcs=interwork --pd "__MICROLIB SETA 1"
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M0+                      \
                  -L --library_type=microlib               \
                  -L --strict -L --summary_stderr -L --map \
                  -L --xref -L --callgraph -L --symbols    \
                  -L --keep=*Handler \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
endif
