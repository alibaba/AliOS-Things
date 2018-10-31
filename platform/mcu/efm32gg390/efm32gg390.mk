NAME := efm32gg390
HOST_OPENOCD :=efm32
$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += sal libc rhino hal vfs


GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192

GLOBAL_INCLUDES += ../../arch/arm/armv7m/gcc/m3

GLOBAL_INCLUDES += include \
				   cmsis/include \
				   EMLib/em_lib/include \
				   EMLib/usb/inc \
				   hal \
				   Utils/include



GLOBAL_INCLUDES += Essentials/include \
				   Essentials/include/bsp \
				   Essentials/include/mcu \
				   Essentials/include/mcu/efm32


$(NAME)_SOURCES := aos/aos.c \
                   aos/soc_impl.c \
                   aos/trace_impl.c \

$(NAME)_SOURCES += Utils/UartTransceiver.c \
				   Utils/RingBuffer.c

$(NAME)_SOURCES += hal/efm32_hal_uart.c \
	               hal/efm32_hal.c \
                   hal/system_efm32gg.c \
                   hal/hw.c \
				   hal/Flash.c \
                   hal/flash_port.c \


$(NAME)_SOURCES += EMLib/em_lib/src/em_acmp.c \
                   EMLib/em_lib/src/em_adc.c \
				   EMLib/em_lib/src/em_aes.c \
				   EMLib/em_lib/src/em_assert.c \
				   EMLib/em_lib/src/em_burtc.c \
				   EMLib/em_lib/src/em_cmu.c \
				   EMLib/em_lib/src/em_dac.c \
				   EMLib/em_lib/src/em_dbg.c \
				   EMLib/em_lib/src/em_dma.c \
				   EMLib/em_lib/src/em_ebi.c \
				   EMLib/em_lib/src/em_emu.c \
				   EMLib/em_lib/src/em_gpio.c \
				   EMLib/em_lib/src/em_i2c.c \
				   EMLib/em_lib/src/em_idac.c \
				   EMLib/em_lib/src/em_int.c \
				   EMLib/em_lib/src/em_lcd.c \
				   EMLib/em_lib/src/em_lesense.c \
				   EMLib/em_lib/src/em_letimer.c \
				   EMLib/em_lib/src/em_leuart.c \
				   EMLib/em_lib/src/em_mpu.c \
				   EMLib/em_lib/src/em_msc.c \
				   EMLib/em_lib/src/em_opamp.c \
				   EMLib/em_lib/src/em_pcnt.c \
				   EMLib/em_lib/src/em_prs.c \
				   EMLib/em_lib/src/em_rmu.c \
				   EMLib/em_lib/src/em_rtc.c \
				   EMLib/em_lib/src/em_system.c \
				   EMLib/em_lib/src/em_timer.c \
				   EMLib/em_lib/src/em_usart.c  \
				   EMLib/em_lib/src/em_vcmp.c \
				   EMLib/em_lib/src/em_wdog.c



$(NAME)_SOURCES += Essentials/source/mcu/efm32/Timer.c \
				   Essentials/source/mcu/efm32/SPI.c \
				   Essentials/source/mcu/efm32/UART.c \
				   Essentials/source/Retcode.c



ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M3 -D__MICROLIB -g -- split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M3 \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m3 \
                 -march=armv7-m  \
                 -mlittle-endian \
                 -mthumb -mthumb-interwork \
                 -w
GLOBAL_CFLAGS  += -D__VFP_FP__
endif

GLOBAL_CFLAGS += -DEFM32GG390F1024

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M3 -g \
				   --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M3 \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m3 \
                   -mlittle-endian \
                   -mthumb \
                   -w
endif



ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M3   \
                  -L --strict \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M3.vfp

else
GLOBAL_LDFLAGS += -mcpu=cortex-m3  \
                  -mlittle-endian  \
                  -mthumb -mthumb-interwork \
                  --specs=nosys.specs \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif

