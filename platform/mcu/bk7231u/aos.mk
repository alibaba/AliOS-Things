NAME := mcu_bk7231u

HOST_OPENOCD := bk7231u

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu bk7231

$(NAME)_COMPONENTS := arch_armv5
$(NAME)_COMPONENTS += newlib_stub rhino yloop
$(NAME)_COMPONENTS += lwip netmgr
$(NAME)_COMPONENTS += libprov

GLOBAL_DEFINES += CONFIG_AOS_UOTA_BREAKPOINT
GLOBAL_DEFINES += CFG_SUPPORT_ALIOS=1

GLOBAL_CFLAGS += -mcpu=arm968e-s           \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w

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

BINS ?=

GLOBAL_LDS_FILES += platform/mcu/bk7231u/bk7231u.ld.S

include ./platform/mcu/bk7231u/beken/beken.mk

$(NAME)_INCLUDES += aos

$(NAME)_SOURCES := aos/aos_main.c
$(NAME)_SOURCES += aos/soc_impl.c

$(NAME)_SOURCES += hal/gpio.c        \
                   hal/wdg.c         \
                   hal/hw.c          \
                   hal/flash.c       \
                   hal/uart.c        \
                   hal/StringUtils.c \
                   hal/wifi_port.c   \
                   hal/beken_rhino.c \
                   hal_init/hal_init.c

ifneq ($(filter umesh,$(COMPONENTS)),)
$(NAME)_SOURCES +=  hal/mesh_wifi_hal.c
endif

GLOBAL_INCLUDES += beken/driver/ble/rw_ble/ip/ble/profiles/AIS/api
$(NAME)_SOURCES += hal/ble.c
$(NAME)_SOURCES += hal/breeze_hal_os.c

ifneq ($(filter pwrmgmt,$(COMPONENTS)),)
$(NAME)_SOURCES +=  hal/pwrmgmt_hal/board_cpu_pwr.c \
                    hal/pwrmgmt_hal/board_cpu_pwr_systick.c \
                    hal/pwrmgmt_hal/board_cpu_pwr_timer.c
endif

GLOBAL_LDFLAGS += -Wl,--wrap=boot_undefined
GLOBAL_LDFLAGS += -Wl,--wrap=boot_pabort
GLOBAL_LDFLAGS += -Wl,--wrap=boot_dabort

