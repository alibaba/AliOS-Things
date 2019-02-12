NAME := nano130ke3bn
HOST_OPENOCD := nano130ke3bn
$(NAME)_TYPE := kernel

#$(NAME)_COMPONENTS += platform/arch/arm/armv6m
#$(NAME)_COMPONENTS += libc rhino kernel.modules.fs.kv 
$(NAME)_COMPONENTS += platform/arch/arm/armv6m
$(NAME)_COMPONENTS += rhino libc kernel.modules.fs.kv

ifeq ($(COMPILER),armcc)

#GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M0 --library_type=microlib -D__MICROLIB -g --split_sections
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M0 --library_type=microlib -D__MICROLIB -g --split_sections

GLOBAL_ASMFLAGS += --cpu=Cortex-M0 -g --library_type=microlib --pd "__MICROLIB SETA 1"
GLOBAL_LDFLAGS  += -L --cpu=Cortex-M0   \
				  -L --library_type=microlib \
                  -L --strict \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes


GLOBAL_LDFLAGS += -L --scatter=platform/mcu/$(NAME)/NANO130.sct

else ifeq ($(COMPILER),iar)

GLOBAL_CFLAGS += --cpu=Cortex-M0 \
                 --cpu_mode=thumb \
                 --endian=little
GLOBAL_ASMFLAGS += --cpu Cortex-M0 \
                   --cpu_mode thumb \
                   --endian little
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M0.vfp
GLOBAL_LDFLAGS += --config NANO130.icf

else

#GLOBAL_CFLAGS += -mcpu=cortex-m0 \
#                 -march=armv6-m  \
#                 -mlittle-endian \
#                 -mthumb -mthumb-interwork \
#                 -w

#GLOBAL_ASMFLAGS += -mcpu=cortex-m0 \
#                   -mlittle-endian \
#                   -w

#GLOBAL_CFLAGS  += -D__VFP_FP__
#GLOBAL_LDFLAGS += -mcpu=cortex-m0  \
#                  -mlittle-endian  \
#                  -mthumb -mthumb-interwork \
#                  --specs=nosys.specs \
#                  $(CLIB_LDFLAGS_NANO_FLOAT)

GLOBAL_CFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
GLOBAL_CFLAGS += -mcpu=cortex-m0 -mfloat-abi=soft -MMD -MP
GLOBAL_CFLAGS += -Wno-format -Wno-incompatible-pointer-types

GLOBAL_ASMFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
#GLOBAL_ASMFLAGS += -D__STARTUP_CLEAR_BSS
GLOBAL_ASMFLAGS += -mcpu=cortex-m0 -mfloat-abi=soft
GLOBAL_INCLUDES += ../../arch/arm/armv6m/gcc/m0/

GLOBAL_LDFLAGS += --specs=nano.specs --specs=nosys.specs
GLOBAL_LDFLAGS += -Wall -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mthumb -mapcs -std=gnu99
GLOBAL_LDFLAGS += -Xlinker --gc-sections -Xlinker -static -Xlinker -z -Xlinker muldefs
GLOBAL_LDFLAGS += -mcpu=cortex-m0 -mfloat-abi=soft
GLOBAL_LDFLAGS += -T platform/mcu/$(NAME)/NANO130.ld

endif

GLOBAL_INCLUDES += \
		Library/CMSIS/Include \
		Library/Device/Nuvoton/Nano100Series/Include \
		Library/StdDriver/inc \
		hal/misc \
		hal/common

BUILT_FULL_DRIVER := no
ifeq ($(BUILT_FULL_DRIVER),yes)
$(NAME)_SOURCES += \
	Library/StdDriver/src/adc.c \
	Library/StdDriver/src/fmc.c \
	Library/StdDriver/src/pdma.c \
	Library/StdDriver/src/scuart.c \
	Library/StdDriver/src/usbd.c \
	Library/StdDriver/src/clk.c \
	Library/StdDriver/src/gpio.c \
	Library/StdDriver/src/pwm.c \
	Library/StdDriver/src/spi.c \
	Library/StdDriver/src/wdt.c \
	Library/StdDriver/src/crc.c \
	Library/StdDriver/src/i2c.c \
	Library/StdDriver/src/retarget.c \
	Library/StdDriver/src/sys.c \
	Library/StdDriver/src/wwdt.c
	Library/StdDriver/src/dac.c \
	Library/StdDriver/src/i2s.c \
	Library/StdDriver/src/rtc.c \
	Library/StdDriver/src/timer.c
	Library/StdDriver/src/ebi.c \
	Library/StdDriver/src/lcd.c \
	Library/StdDriver/src/sc.c \
	Library/StdDriver/src/uart.c \
	Library/Device/Nuvoton/Nano100Series/Source/system_Nano100Series.c
else
$(NAME)_SOURCES += \
	Library/StdDriver/src/adc.c \
	Library/StdDriver/src/fmc.c \
	Library/StdDriver/src/pdma.c \
	Library/StdDriver/src/clk.c \
	Library/StdDriver/src/gpio.c \
	Library/StdDriver/src/pwm.c \
	Library/StdDriver/src/spi.c \
	Library/StdDriver/src/wdt.c \
	Library/StdDriver/src/i2c.c \
	Library/StdDriver/src/retarget.c \
	Library/StdDriver/src/sys.c \
	Library/StdDriver/src/dac.c \
	Library/StdDriver/src/rtc.c \
	Library/StdDriver/src/timer.c \
	Library/StdDriver/src/uart.c \
	Library/Device/Nuvoton/Nano100Series/Source/system_Nano100Series.c
endif

# AliOS-thing SoC&HAL implementation
$(NAME)_SOURCES += \
		aos/soc_impl.c \
		aos/trace_impl.c \
		aos/aos.c \
		hal/common/pinmap_common.c \
		hal/common/PeripheralPins.c \
		hal/common/pinmap.c \
		hal/misc/nu_modutil.c \
		hal/hw.c \
		hal/hal_uart.c \
		hal/hal_timer.c \
		hal/hal_flash.c \
		hal/hal_dummy.c
