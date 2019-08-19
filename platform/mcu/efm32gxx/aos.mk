NAME := mcu_efm32gxx

HOST_OPENOCD := efm32gxx

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu efm32gxx

$(NAME)_COMPONENTS += arch_armv7m
$(NAME)_COMPONENTS += rhino

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_INCLUDES += .                 \
                   drivers/emlib/inc \
                   CMSIS/Include     \
                   hal               \
                   include

$(NAME)_SOURCES := aos/soc_impl.c                \
                   main/main.c                   \
                   system_efm32g.c               \
                   hal/hal_flash.c               \
                   hal/hal_uart.c                \
                   hal/util_buf.c                \
                   drivers/emlib/src/em_cmu.c    \
                   drivers/emlib/src/em_emu.c    \
                   drivers/emlib/src/em_gpio.c   \
                   drivers/emlib/src/em_usart.c  \
                   drivers/emlib/src/em_rtc.c    \
                   drivers/emlib/src/em_system.c \
                   drivers/emlib/src/em_msc.c

ifeq ($(COMPILER),armcc)

else ifeq ($(COMPILER),iar)
$(NAME)_SOURCES += iar/startup_efm32g.c
else
$(NAME)_SOURCES += gcc/startup_efm32g.c
GLOBAL_CFLAGS   += -mcpu=cortex-m3           \
                   -mthumb -mthumb-interwork \
                   -mlittle-endian
GLOBAL_CFLAGS += -w

GLOBAL_LDFLAGS += -mcpu=cortex-m3           \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian           \
                  --specs=nosys.specs       \
                  $(CLIB_LDFLAGS_NANO_FLOAT)
endif

ifeq ($(COMPILER),armcc)

else ifeq ($(COMPILER),iar)

else
GLOBAL_LDFLAGS += -T platform/mcu/efm32gxx/gcc/efm32g.ld -lm
endif
