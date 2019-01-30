NAME := m487jidae
HOST_OPENOCD := m487jidae
$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += libc rhino hal rhino.vfs digest_algorithm kernel.modules.fs.kv

GLOBAL_INCLUDES += \
				   lwIP\include\netif \
				   lwIP/include \
                   Library/CMSIS/Include \
                   Library/Device/Nuvoton/M480/Include \
                   Library/StdDriver/inc \
                   hal/misc \
                   hal/common

ifeq ($(WIFI),1)
GLOBAL_INCLUDES += ../../../network/sal/wifi/esp8266_aos
endif
  
#ToDo
#Comment Library/StdDriver/src/crc.c, due to out/helloworld@numaker-pfm-m487/Modules/utility/digest_algorithm/crc.o?

BUILT_FULL_DRIVER := no
ifeq ($(BUILT_FULL_DRIVER),yes)
$(NAME)_SOURCES += \
                   Library/StdDriver/src/acmp.c \
                   Library/StdDriver/src/bpwm.c \
                   Library/StdDriver/src/can.c \
                   Library/StdDriver/src/clk.c \
                   Library/StdDriver/src/crypto.c \
                   Library/StdDriver/src/dac.c \
                   Library/StdDriver/src/eadc.c \
                   Library/StdDriver/src/ebi.c \
                   Library/StdDriver/src/ecap.c \
                   Library/StdDriver/src/emac.c \
                   Library/StdDriver/src/epwm.c \
                   Library/StdDriver/src/fmc.c \
                   Library/StdDriver/src/gpio.c \
                   Library/StdDriver/src/hsusbd.c \
                   Library/StdDriver/src/i2c.c \
                   Library/StdDriver/src/i2s.c \
                   Library/StdDriver/src/pdma.c \
                   Library/StdDriver/src/qei.c \
                   Library/StdDriver/src/qspi.c \
                   Library/StdDriver/src/retarget.c \
                   Library/StdDriver/src/rtc.c \
                   Library/StdDriver/src/sc.c \
                   Library/StdDriver/src/scuart.c \
                   Library/StdDriver/src/sdh.c \
                   Library/StdDriver/src/spi.c \
                   Library/StdDriver/src/spim.c \
                   Library/StdDriver/src/sys.c \
                   Library/StdDriver/src/timer.c \
                   Library/StdDriver/src/timer_pwm.c \
                   Library/StdDriver/src/uart.c \
                   Library/StdDriver/src/usbd.c \
                   Library/StdDriver/src/usci_i2c.c \
                   Library/StdDriver/src/usci_spi.c \
                   Library/StdDriver/src/usci_uart.c \
                   Library/StdDriver/src/wdt.c \
                   Library/StdDriver/src/wwdt.c \
                   Library/Device/Nuvoton/M480/Source/system_M480.c
else
$(NAME)_SOURCES += \
                   Library/StdDriver/src/clk.c \
                   Library/StdDriver/src/crypto.c \
                   Library/StdDriver/src/dac.c \
                   Library/StdDriver/src/eadc.c \
                   Library/StdDriver/src/epwm.c \
                   Library/StdDriver/src/fmc.c \
                   Library/StdDriver/src/gpio.c \
                   Library/StdDriver/src/i2c.c \
				   Library/StdDriver/src/i2s.c \
                   Library/StdDriver/src/pdma.c \
                   Library/StdDriver/src/qspi.c \
                   Library/StdDriver/src/retarget.c \
                   Library/StdDriver/src/rtc.c \
                   Library/StdDriver/src/sdh.c \
                   Library/StdDriver/src/spi.c \
                   Library/StdDriver/src/spim.c \
                   Library/StdDriver/src/sys.c \
                   Library/StdDriver/src/timer.c \
                   Library/StdDriver/src/timer_pwm.c \
                   Library/StdDriver/src/uart.c \
                   Library/StdDriver/src/wdt.c \
                   Library/Device/Nuvoton/M480/Source/system_M480.c
endif

# AliOS-thing SoC&HAL implementation
$(NAME)_SOURCES += aos/soc_impl.c \
                   aos/trace_impl.c \
                   aos/aos.c \
                   hal/common/pinmap_common.c \
                   hal/common/PeripheralPins.c \
                   hal/common/pinmap.c \
                   hal/misc/nu_modutil.c \
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
                   hal/hal_dma.c \
                   hal/ota_port.c \
                   hal/ota_spimflash.c
                   
ifeq ($(ETHERNET),1)
$(NAME)_SOURCES += 	lwIP/netif/ethernetif.c \
					lwIP/netif/m480_eth.c \
                    hal/eth_port.c 
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

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/mcu/$(NAME)/M487.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/mcu/$(NAME)/M487.icf
else
GLOBAL_LDFLAGS += -T platform/mcu/$(NAME)/M487.ld
endif
