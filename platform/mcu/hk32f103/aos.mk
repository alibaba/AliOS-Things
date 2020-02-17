NAME := mcu_hk32f103
HOST_OPENOCD := hk32f103

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu hk32f103

$(NAME)_COMPONENTS += arch_armv7m rhino

GLOBAL_DEFINES += USE_HAL_DRIVER STM32F103xB

GLOBAL_INCLUDES += drivers/CMSIS/Include \
                   drivers/HK32F1xx_HAL_Driver/Inc

$(NAME)_SOURCES += drivers/system_hk32f1xx.c

$(NAME)_PREBUILT_LIBRARY := drivers/HK32F1xx_HAL_Driver/Src/driver.lib

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M3 --apcs=interwork -D__MICROLIB --li -g --split_sections
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M3 --li -g --apcs=interwork --pd "__MICROLIB SETA 1"
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M3                      \
                  -L --library_type=microlib               \
                  -L --strict -L --summary_stderr -L --map \
                  -L --xref -L --callgraph -L --symbols    \
                  -L --keep=*Handler \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
endif
