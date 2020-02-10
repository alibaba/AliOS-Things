NAME := mcu_bk7231

HOST_OPENOCD := bk7231

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := driver & sdk for platform/mcu bk7231

$(NAME)_COMPONENTS := arch_armv5
$(NAME)_COMPONENTS += newlib_stub rhino yloop
LWIP := 1
$(NAME)_COMPONENTS += lwip netmgr
$(NAME)_COMPONENTS += libprov

GLOBAL_DEFINES += CONFIG_AOS_UOTA_BREAKPOINT

GLOBAL_CFLAGS += -mcpu=arm968e-s           \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_INCLUDES += beken/lwip-2.0.2/port \
                   beken/common          \
                   beken/app/config      \
                   beken/func/include    \
                   beken/os/include      \
                   beken/driver/include  \
                   beken/driver/common   \
                   beken/ip/common       \
                   beken

GLOBAL_LDFLAGS += -mcpu=arm968e-s          \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian           \
                 --specs=nosys.specs       \
                 -nostartfiles             \
                 $(CLIB_LDFLAGS_NANO_FLOAT)

MBINS ?=

GLOBAL_LDS_FILES += platform/mcu/bk7231/bk7231.ld.S

$(NAME)_INCLUDES += aos

$(NAME)_SOURCES :=  aos/aos_main.c
$(NAME)_SOURCES +=  aos/soc_impl.c

$(NAME)_SOURCES += hal/gpio.c        \
                   hal/wdg.c         \
                   hal/hw.c          \
                   hal/flash.c       \
                   hal/uart.c        \
                   hal/ringbuf.c     \
                   hal/StringUtils.c \
                   hal/wifi_port.c   \
                   hal/beken_rhino.c

include ./platform/mcu/bk7231/hal_init/hal_init.mk

$(NAME)_PREBUILT_LIBRARY := beken.a

EXTRA_TARGET_MAKEFILES += $($(HOST_MCU_FAMILY)_LOCATION)/gen_image_bin.mk

