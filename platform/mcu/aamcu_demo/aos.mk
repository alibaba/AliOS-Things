NAME := mcu_aamcu_demo

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu aamcu_demo

$(NAME)_COMPONENTS += arch_armv7m
$(NAME)_COMPONENTS += newlib_stub rhino

GLOBAL_INCLUDES += drivers/ \
                   hal/include

$(NAME)_SOURCES := drivers/driver_demo.c \
                   hal/hal_uart.c        \
                   hal/hal_flash.c       \
                   hal/hal_sd.c          \
                   hal/hal_adc.c         \
                   hal/hal_i2c.c         \
                   hal/hal_gpio.c        \
                   hal/hal_rtc.c         \
                   hal/hal_spi.c         \
                   hal/hal_qspi.c        \
                   hal/hal_nand.c        \
                   hal/hal_nor.c         \
                   hal/hw.c

ifeq ($(COMPILER),armcc)
GLOBAL_CFLAGS   += --c99 --cpu=Cortex-M4 --apcs=/hardfp --fpu=vfpv4_sp_d16 -D__MICROLIB -g --split_sections
else ifeq ($(COMPILER),iar)
GLOBAL_CFLAGS += --cpu=Cortex-M4  \
                 --cpu_mode=thumb \
                 --endian=little
else
GLOBAL_CFLAGS += -mcpu=cortex-m4           \
                 -mlittle-endian           \
                 -mthumb -mthumb-interwork \
                 -mfloat-abi=hard          \
                 -mfpu=fpv4-sp-d16         \
                 -w
GLOBAL_CFLAGS  += -D__VFP_FP__
GLOBAL_CXXFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
endif

ifeq ($(COMPILER),armcc)
GLOBAL_ASMFLAGS += --cpu=Cortex-M4 -g --apcs=/hardfp --fpu=vfpv4_sp_d16 --library_type=microlib --pd "__MICROLIB SETA 1"
else ifeq ($(COMPILER),iar)
GLOBAL_ASMFLAGS += --cpu Cortex-M4  \
                   --cpu_mode thumb \
                   --endian little
else
GLOBAL_ASMFLAGS += -mcpu=cortex-m4   \
                   -mlittle-endian   \
                   -mthumb           \
                   -mfloat-abi=hard  \
                   -mfpu=fpv4-sp-d16 \
                   -w
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --cpu=Cortex-M4                    \
                  -L --fpu=vfpv4_sp_d16                 \
                  -L --apcs=/hardfp                     \
                  -L --strict                           \
                  -L --xref -L --callgraph -L --symbols \
                  -L --info=sizes -L --info=totals -L --info=unused -L --info=veneers -L --info=summarysizes
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --silent --cpu=Cortex-M4.vfp

else
GLOBAL_LDFLAGS += -mcpu=cortex-m4           \
                  -mlittle-endian           \
                  -mthumb -mthumb-interwork \
                  --specs=nosys.specs       \
                  -mfloat-abi=hard          \
                  -mfpu=fpv4-sp-d16         \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif

include $($(NAME)_LOCATION)/$(HOST_MCU_NAME).mk
