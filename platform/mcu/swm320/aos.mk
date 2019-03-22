NAME := mcu_swm320
HOST_OPENOCD := swm320

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu swm320

$(NAME)_COMPONENTS += arch_armv7m rhino

GLOBAL_DEFINES += CONFIG_NO_TCPIP

GLOBAL_INCLUDES += drivers/        \
                   drivers/CMSIS/CoreSupport \
                   drivers/CMSIS/DeviceSupport \
                   drivers/SWM320_StdPeriph_Driver

$(NAME)_SOURCES += hal/hw.c \
                   hal/hal_flash_swm320.c \
                   hal/hal_uart_swm320.c \
                   drivers/CMSIS/DeviceSupport/system_SWM320.c \
                   drivers/SWM320_StdPeriph_Driver/SWM320_flash.c \
                   drivers/SWM320_StdPeriph_Driver/SWM320_gpio.c \
                   drivers/SWM320_StdPeriph_Driver/SWM320_port.c \
                   drivers/SWM320_StdPeriph_Driver/SWM320_uart.c

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M4 --apcs=interwork -D__MICROLIB --li -g --split_sections
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M4 --li -g --apcs=interwork --pd "__MICROLIB SETA 1"
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M4                       \
                  -L --library_type=microlib               \
                  -L --strict -L --summary_stderr -L --map \
                  -L --xref -L --callgraph -L --symbols    \
                  -L --keep=*Handler \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
endif
