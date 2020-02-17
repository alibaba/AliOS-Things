NAME := mcu_mvs_ap80xx
HOST_OPENOCD := mvs_ap80xx

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu mvs_ap80xx

$(NAME)_COMPONENTS += arch_armv7m rhino

GLOBAL_DEFINES += RHINO_CONFIG_NEXT_INTRPT_TICKS=100u

GLOBAL_INCLUDES += drivers/inc \
                   drivers/cmsis

$(NAME)_SOURCES += hal/hw.c \
                   drivers/retarget.c

$(NAME)_PREBUILT_LIBRARY := drivers/driver.lib

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M3 -D__MICROLIB --li --apcs=interwork -g -O0 --split_sections
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M3 --li -g --apcs=interwork
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M3 -L --library_type=microlib -L --strict -L --summary_stderr -L --map \
                  -L --diag_suppress=L6314W,L6319W \
                  -L --xref -L --callgraph -L --symbols    \
                  -L --info=sizes -L --info=totals -L --info=summarysizes
endif
