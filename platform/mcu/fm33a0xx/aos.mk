NAME := mcu_fm33a0xx
HOST_OPENOCD := fm33a0xx

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu fm33a0xx

$(NAME)_COMPONENTS += arch_armv6m rhino osal_aos

GLOBAL_DEFINES += __DEBUG

GLOBAL_INCLUDES += drivers/library/include \
                   drivers/cmsis

$(NAME)_SOURCES += hal/hw.c \
                   hal/flash.c \
                   hal/gpio.c \
                   hal/rng.c \
                   hal/rtc.c \
                   hal/spi.c \
                   hal/uart.c \
                   hal/wdg.c \
                   drivers/library/source/fm33a0xx_aes.c \
                   drivers/library/source/fm33a0xx_anac.c \
                   drivers/library/source/fm33a0xx_btim.c \
                   drivers/library/source/fm33a0xx_crc.c \
                   drivers/library/source/fm33a0xx_dma.c \
                   drivers/library/source/fm33a0xx_etim.c \
                   drivers/library/source/fm33a0xx_flash.c \
                   drivers/library/source/fm33a0xx_gpio.c \
                   drivers/library/source/fm33a0xx_hspi.c \
                   drivers/library/source/fm33a0xx_i2c.c \
                   drivers/library/source/fm33a0xx_iwdt.c \
                   drivers/library/source/fm33a0xx_lcd.c \
                   drivers/library/source/fm33a0xx_lptim.c \
                   drivers/library/source/fm33a0xx_pmu.c \
                   drivers/library/source/fm33a0xx_rcc.c \
                   drivers/library/source/fm33a0xx_rtc.c \
                   drivers/library/source/fm33a0xx_scu.c \
                   drivers/library/source/fm33a0xx_spi.c \
                   drivers/library/source/fm33a0xx_trng.c \
                   drivers/library/source/fm33a0xx_u7816.c \
                   drivers/library/source/fm33a0xx_uart.c \
                   drivers/library/source/fm33a0xx_wwdt.c \
                   drivers/system_FM33A0XX.c


ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M0+ --apcs=interwork -D__MICROLIB -g -O1 --split_sections
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M0+ --li -g --apcs=interwork --pd "__MICROLIB SETA 1"
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M0+                      \
                  -L --library_type=microlib               \
                  -L --strict -L --summary_stderr -L --map \
                  -L --xref -L --callgraph -L --symbols    \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
endif
