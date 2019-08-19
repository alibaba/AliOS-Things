NAME := mcu_nano130ke3bn
HOST_OPENOCD := nano130ke3bn

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu nano130ke3bn

$(NAME)_COMPONENTS += arch_armv6m rhino

GLOBAL_INCLUDES += drivers/Library/CMSIS/Include \
                   drivers/Library/Device/Nuvoton/Nano100Series/Include \
                   drivers/Library/StdDriver/inc \
                   drivers/misc \
                   drivers/common

BUILT_FULL_DRIVER := no
ifeq ($(BUILT_FULL_DRIVER),yes)
$(NAME)_SOURCES += \
                  drivers/Library/StdDriver/src/adc.c \
                  drivers/Library/StdDriver/src/fmc.c \
                  drivers/Library/StdDriver/src/pdma.c \
                  drivers/Library/StdDriver/src/scuart.c \
                  drivers/Library/StdDriver/src/usbd.c \
                  drivers/Library/StdDriver/src/clk.c \
                  drivers/Library/StdDriver/src/gpio.c \
                  drivers/Library/StdDriver/src/pwm.c \
                  drivers/Library/StdDriver/src/spi.c \
                  drivers/Library/StdDriver/src/wdt.c \
                  drivers/Library/StdDriver/src/crc.c \
                  drivers/Library/StdDriver/src/i2c.c \
                  drivers/Library/StdDriver/src/retarget.c \
                  drivers/Library/StdDriver/src/sys.c \
                  drivers/Library/StdDriver/src/wwdt.c \
                  drivers/Library/StdDriver/src/dac.c \
                  drivers/Library/StdDriver/src/rtc.c \
                  drivers/Library/StdDriver/src/timer.c \
                  drivers/Library/StdDriver/src/ebi.c \
                  drivers/Library/StdDriver/src/lcd.c \
                  drivers/Library/StdDriver/src/sc.c \
                  drivers/Library/StdDriver/src/uart.c \
                  drivers/Library/Device/Nuvoton/Nano100Series/Source/system_Nano100Series.c
else
$(NAME)_SOURCES += \
                  drivers/Library/StdDriver/src/adc.c \
                  drivers/Library/StdDriver/src/fmc.c \
                  drivers/Library/StdDriver/src/pdma.c \
                  drivers/Library/StdDriver/src/clk.c \
                  drivers/Library/StdDriver/src/gpio.c \
                  drivers/Library/StdDriver/src/pwm.c \
                  drivers/Library/StdDriver/src/spi.c \
                  drivers/Library/StdDriver/src/wdt.c \
                  drivers/Library/StdDriver/src/i2c.c \
                  drivers/Library/StdDriver/src/retarget.c \
                  drivers/Library/StdDriver/src/sys.c \
                  drivers/Library/StdDriver/src/dac.c \
                  drivers/Library/StdDriver/src/rtc.c \
                  drivers/Library/StdDriver/src/timer.c \
                  drivers/Library/StdDriver/src/uart.c \
                  drivers/Library/Device/Nuvoton/Nano100Series/Source/system_Nano100Series.c
endif

# AliOS-thing HAL implementation
$(NAME)_SOURCES += drivers/common/pinmap_common.c \
                   drivers/common/PeripheralPins.c \
                   drivers/common/pinmap.c \
                   drivers/misc/nu_modutil.c \
                   hal/hw.c \
                   hal/hal_uart.c \
                   hal/hal_timer.c \
                   hal/hal_flash.c


ifeq ($(COMPILER),armcc)

#GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M0 --library_type=microlib -D__MICROLIB -g --split_sections
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M0 --library_type=microlib -D__MICROLIB -g --split_sections

GLOBAL_ASMFLAGS += --cpu=Cortex-M0 -g --library_type=microlib --pd "__MICROLIB SETA 1"
GLOBAL_LDFLAGS  += -L --cpu=Cortex-M0   \
                          -L --library_type=microlib \
                  -L --strict \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes

else ifeq ($(COMPILER),iar)

GLOBAL_CFLAGS += --cpu=Cortex-M0 \
                 --cpu_mode=thumb \
                 --endian=little
GLOBAL_ASMFLAGS += --cpu Cortex-M0 \
                   --cpu_mode thumb \
                   --endian little
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M0.vfp

else

GLOBAL_CFLAGS += -w -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
GLOBAL_CFLAGS += -mcpu=cortex-m0 -mfloat-abi=soft -MMD -MP
GLOBAL_CFLAGS += -Wno-format -Wno-incompatible-pointer-types

GLOBAL_ASMFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
#GLOBAL_ASMFLAGS += -D__STARTUP_CLEAR_BSS
GLOBAL_ASMFLAGS += -mcpu=cortex-m0 -mfloat-abi=soft

GLOBAL_LDFLAGS += --specs=nano.specs --specs=nosys.specs
GLOBAL_LDFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
GLOBAL_LDFLAGS += -Xlinker --gc-sections -Xlinker -static -Xlinker -z -Xlinker muldefs
GLOBAL_LDFLAGS += -mcpu=cortex-m0 -mfloat-abi=soft
endif
