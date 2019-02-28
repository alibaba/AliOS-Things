NAME := mcu_sscp131
HOST_OPENOCD := sscp131

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu Eastsoft p131

$(NAME)_COMPONENTS += arch_armv7m rhino

GLOBAL_DEFINES += __PRINTF_USE_UART0__ CONFIG_NO_TCPIP

GLOBAL_INCLUDES += drivers/        \
                   drivers/library/Include \
                   drivers/cmsis

$(NAME)_SOURCES += hal/hw.c \
	           hal/hal_uart.c \
                   drivers/library/Source/lib_aes.c \
                   drivers/library/Source/lib_crc.c \
                   drivers/library/Source/lib_dma.c \
                   drivers/library/Source/lib_ecc.c \
                   drivers/library/Source/lib_flashiap.c \
                   drivers/library/Source/lib_gpio.c \
                   drivers/library/Source/lib_iic.c \
                   drivers/library/Source/lib_printf.c \
                   drivers/library/Source/lib_scs.c \
                   drivers/library/Source/lib_scu.c \
                   drivers/library/Source/lib_spi.c \
                   drivers/library/Source/lib_timer.c \
                   drivers/library/Source/lib_trng.c \
                   drivers/library/Source/lib_uart.c \
                   drivers/library/Source/lib_wdt.c \
                   drivers/library/Source/systick.c \
                   drivers/system_p131.c


ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --bitband --c99 --cpu=Cortex-M3 --apcs=interwork -D__MICROLIB -g -O0 --split_sections
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M3 --li -g --apcs=interwork --pd "__MICROLIB SETA 1"
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M3                       \
                  -L --library_type=microlib               \
                  -L --strict -L --summary_stderr -L --map \
                  -L --xref -L --callgraph -L --symbols    \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
endif
