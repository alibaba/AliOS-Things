NAME := mcu_bk7231s

HOST_OPENOCD := bk7231s

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu bk7231s

$(NAME)_COMPONENTS := arch_armv5
$(NAME)_COMPONENTS += newlib_stub rhino yloop alicrypto debug
$(NAME)_COMPONENTS += lwip netmgr
$(NAME)_COMPONENTS += libprov

GLOBAL_DEFINES += CONFIG_AOS_UOTA_BREAKPOINT
GLOBAL_DEFINES += CONFIG_AOS_CLI_BOARD
GLOBAL_DEFINES += CFG_SUPPORT_ALIOS=1

GLOBAL_CFLAGS += -mcpu=arm968e-s           \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w

$(NAME)_PREBUILT_LIBRARY :=

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_INCLUDES +=  beken/alios/entry \
                    beken/alios/lwip-2.0.2/port \
                    beken/alios/os/include \
                    beken/common \
                    beken/app \
                    beken/app/config \
                    beken/func/include \
                    beken/func/uart_debug \
                    beken/driver/include \
                    beken/driver/common \
                    beken/ip/common

GLOBAL_LDFLAGS += -mcpu=arm968e-s           \
                  -march=armv5te            \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian           \
                  --specs=nosys.specs       \
                  -nostartfiles             \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

PING_PONG_OTA := 0
ifeq ($(PING_PONG_OTA),1)
GLOBAL_DEFINES += CONFIG_PING_PONG_OTA
AOS_IMG1_XIP1_LD_FILE += platform/mcu/bk7231s/bk7231s.ld
AOS_IMG2_XIP2_LD_FILE += platform/mcu/bk7231s/bk7231s_ex.ld
else
GLOBAL_LDS_FILES += platform/mcu/bk7231s/bk7231s.ld
endif

$(NAME)_INCLUDES += aos

$(NAME)_SOURCES += aos/aos_main.c
$(NAME)_SOURCES += aos/soc_impl.c

$(NAME)_SOURCES += hal/gpio.c        \
                   hal/wdg.c         \
                   hal/hw.c          \
                   hal/flash.c       \
                   hal/uart.c        \
                   hal/StringUtils.c \
                   hal/wifi_port.c   \
                   hal/beken_rhino.c \
                   hal/pwm.c \
                   hal_init/hal_init.c

$(NAME)_SOURCES += hal/pwrmgmt_hal/board_cpu_pwr.c \
                   hal/pwrmgmt_hal/board_cpu_pwr_systick.c \
                   hal/pwrmgmt_hal/board_cpu_pwr_timer.c

#Beken ip
$(NAME)_PREBUILT_LIBRARY += beken/ip/ip.a
#$(NAME)_COMPONENTS += ip

#Beken entry
$(NAME)_COMPONENTS += entry

include ./platform/mcu/bk7231s/beken/beken.mk

EXTRA_TARGET_MAKEFILES += $($(HOST_MCU_FAMILY)_LOCATION)/gen_image_bin.mk

