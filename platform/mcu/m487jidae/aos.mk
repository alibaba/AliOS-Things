NAME := mcu_m487jidae
HOST_OPENOCD := m487jidae

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu m487jidae

$(NAME)_COMPONENTS += arch_armv7m rhino

GLOBAL_INCLUDES += drivers/lwIP/include/netif \
                   drivers/lwIP/include \
                   drivers/Library/CMSIS/Include \
                   drivers/Library/Device/Nuvoton/M480/Include \
                   drivers/Library/StdDriver/inc \
                   drivers/misc \
                   drivers/common

BUILT_FULL_DRIVER := no
ifeq ($(BUILT_FULL_DRIVER),yes)
$(NAME)_SOURCES += \
                   drivers/Library/StdDriver/src/acmp.c \
                   drivers/Library/StdDriver/src/bpwm.c \
                   drivers/Library/StdDriver/src/can.c \
                   drivers/Library/StdDriver/src/clk.c \
                   drivers/Library/StdDriver/src/crypto.c \
                   drivers/Library/StdDriver/src/dac.c \
                   drivers/Library/StdDriver/src/eadc.c \
                   drivers/Library/StdDriver/src/ebi.c \
                   drivers/Library/StdDriver/src/ecap.c \
                   drivers/Library/StdDriver/src/emac.c \
                   drivers/Library/StdDriver/src/epwm.c \
                   drivers/Library/StdDriver/src/fmc.c \
                   drivers/Library/StdDriver/src/gpio.c \
                   drivers/Library/StdDriver/src/hsusbd.c \
                   drivers/Library/StdDriver/src/i2c.c \
                   drivers/Library/StdDriver/src/i2s.c \
                   drivers/Library/StdDriver/src/pdma.c \
                   drivers/Library/StdDriver/src/qei.c \
                   drivers/Library/StdDriver/src/qspi.c \
                   drivers/Library/StdDriver/src/retarget.c \
                   drivers/Library/StdDriver/src/rtc.c \
                   drivers/Library/StdDriver/src/sc.c \
                   drivers/Library/StdDriver/src/scuart.c \
                   drivers/Library/StdDriver/src/sdh.c \
                   drivers/Library/StdDriver/src/spi.c \
                   drivers/Library/StdDriver/src/spim.c \
                   drivers/Library/StdDriver/src/sys.c \
                   drivers/Library/StdDriver/src/timer.c \
                   drivers/Library/StdDriver/src/timer_pwm.c \
                   drivers/Library/StdDriver/src/uart.c \
                   drivers/Library/StdDriver/src/usbd.c \
                   drivers/Library/StdDriver/src/usci_i2c.c \
                   drivers/Library/StdDriver/src/usci_spi.c \
                   drivers/Library/StdDriver/src/usci_uart.c \
                   drivers/Library/StdDriver/src/wdt.c \
                   drivers/Library/StdDriver/src/wwdt.c \
                   drivers/Library/Device/Nuvoton/M480/Source/system_M480.c
else
$(NAME)_SOURCES += \
                   drivers/Library/StdDriver/src/clk.c \
                   drivers/Library/StdDriver/src/crypto.c \
                   drivers/Library/StdDriver/src/dac.c \
                   drivers/Library/StdDriver/src/eadc.c \
                   drivers/Library/StdDriver/src/epwm.c \
                   drivers/Library/StdDriver/src/fmc.c \
                   drivers/Library/StdDriver/src/gpio.c \
                   drivers/Library/StdDriver/src/i2c.c \
                   drivers/Library/StdDriver/src/pdma.c \
                   drivers/Library/StdDriver/src/qspi.c \
                   drivers/Library/StdDriver/src/retarget.c \
                   drivers/Library/StdDriver/src/rtc.c \
                   drivers/Library/StdDriver/src/sdh.c \
                   drivers/Library/StdDriver/src/spi.c \
                   drivers/Library/StdDriver/src/spim.c \
                   drivers/Library/StdDriver/src/sys.c \
                   drivers/Library/StdDriver/src/timer.c \
                   drivers/Library/StdDriver/src/timer_pwm.c \
                   drivers/Library/StdDriver/src/uart.c \
                   drivers/Library/StdDriver/src/wdt.c \
                   drivers/Library/Device/Nuvoton/M480/Source/system_M480.c
endif

# AliOS-thing HAL implementation
$(NAME)_SOURCES += drivers/common/pinmap_common.c \
                   drivers/common/PeripheralPins.c \
                   drivers/common/pinmap.c \
                   drivers/misc/nu_modutil.c \
                   drivers/common/CheckSumUtils.c \
                   hal/hw.c \
                   hal/hal_uart.c \
                   hal/hal_adc.c \
                   hal/hal_rng.c \
                   hal/hal_rtc.c \
                   hal/hal_sd.c \
                   hal/hal_spi.c \
                   hal/hal_timer.c \
                   hal/hal_wdg.c \
                   hal/hal_dac.c \
                   hal/hal_gpio.c \
                   hal/hal_i2c.c \
                   hal/hal_interrupt.c \
                   hal/hal_pwm.c \
                   hal/hal_flash.c \
                   hal/hal_qspi.c \
                   hal/hal_dma.c

ifeq ($(ETHERNET),1)
$(NAME)_SOURCES += drivers/lwIP/netif/m480_eth.c
$(NAME)_SOURCES += hal/eth_port.c
endif

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M4 --apcs=/hardfp --fpu=vfpv4_sp_d16 -D__MICROLIB -g --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M4 \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m4 \
                 -march=armv7-m  \
                 -mlittle-endian \
                 -mthumb -mthumb-interwork \
                 -mfloat-abi=hard \
                 -mfpu=fpv4-sp-d16 \
                 -w
GLOBAL_CFLAGS  += -D__VFP_FP__
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M4 -g --apcs=/hardfp --fpu=vfpv4_sp_d16 --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M4 \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m4 \
                   -mlittle-endian \
                   -mthumb \
                   -mfloat-abi=hard \
                   -mfpu=fpv4-sp-d16 \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M4   \
                  -L --fpu=vfpv4_sp_d16 \
                  -L --apcs=/hardfp \
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
                  -mfloat-abi=hard \
                  -mfpu=fpv4-sp-d16 \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif

ifeq ($(HOST_OS),Win32)
#EXTRA_TARGET_MAKEFILES +=  $($(HOST_MCU_FAMILY)_LOCATION)/gen_image_bin.mk
endif
