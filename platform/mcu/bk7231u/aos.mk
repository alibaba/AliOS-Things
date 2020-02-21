NAME := mcu_bk7231u

HOST_OPENOCD := bk7231u

$(NAME)_MBINS_TYPE := kernel
<<<<<<< HEAD
<<<<<<< HEAD
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu bk7231
=======
$(NAME)_VERSION    := 1.0.1
=======
$(NAME)_VERSION    := 1.0.2
>>>>>>> 0952e7845 (BugID:24814458:upgrade component version for publish)
$(NAME)_SUMMARY    := driver & sdk for platform/mcu bk7231u
>>>>>>> 17c9a0c98 (BugID:22083675:update component version for component release)

$(NAME)_COMPONENTS := arch_armv5
$(NAME)_COMPONENTS += newlib_stub rhino yloop
$(NAME)_COMPONENTS += lwip netmgr debug
$(NAME)_COMPONENTS += libprov

GLOBAL_DEFINES += CONFIG_AOS_UOTA_BREAKPOINT

GLOBAL_CFLAGS += -mcpu=arm968e-s           \
                 -mthumb -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -w

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS += -Wno-unused-value -Wno-strict-aliasing

GLOBAL_INCLUDES += beken/lwip-2.0.2/port \
                   beken/common          \
                   beken/app/config      \
                   beken/func/include    \
                   beken/os/include      \
                   beken/driver/include  \
                   beken/driver/common   \
                   beken/ip/common       \
                   beken

GLOBAL_LDFLAGS += -mcpu=arm968e-s           \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian           \
                  --specs=nosys.specs       \
                  -nostartfiles             \
                  $(CLIB_LDFLAGS_NANO_FLOAT)

BINS ?=

PING_PONG_OTA := 1
ifeq ($(PING_PONG_OTA),1)
GLOBAL_DEFINES += CONFIG_PING_PONG_OTA
AOS_IMG1_XIP1_LD_FILE += platform/mcu/bk7231u/bk7231u.ld.S
AOS_IMG2_XIP2_LD_FILE += platform/mcu/bk7231u/bk7231u_ex.ld.S
else
GLOBAL_LDS_FILES += platform/mcu/bk7231u/bk7231u.ld.S
endif

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
<<<<<<< HEAD
                   hal/beken_rhino.c
=======
                   hal/beken_rhino.c \
                   hal_init/hal_init.c \
                   hal/ota.c
>>>>>>> c9aad2eb5 (BugID:20262438: OTA refactor for 2.2.1)

include ./platform/mcu/bk7231u/hal_init/hal_init.mk

#ifneq (,$(filter umesh,$(COMPONENTS)))
$(NAME)_SOURCES +=  hal/mesh_wifi_hal.c
#endif

$(NAME)_PREBUILT_LIBRARY := beken.a


<<<<<<< HEAD
GLOBAL_LDFLAGS += -Wl,--wrap=boot_undefined
GLOBAL_LDFLAGS += -Wl,--wrap=boot_pabort
GLOBAL_LDFLAGS += -Wl,--wrap=boot_dabort
=======
$(NAME)_SOURCES +=  hal/pwrmgmt_hal/board_cpu_pwr.c \
                    hal/pwrmgmt_hal/board_cpu_pwr_systick.c \
                    hal/pwrmgmt_hal/board_cpu_pwr_timer.c
>>>>>>> e278ce31f (BugID:18744724:add lowpower suspend/resume api)

GLOBAL_LDFLAGS += -Wl,--wrap=boot_undefined
GLOBAL_LDFLAGS += -Wl,--wrap=boot_pabort
GLOBAL_LDFLAGS += -Wl,--wrap=boot_dabort

EXTRA_TARGET_MAKEFILES += $($(HOST_MCU_FAMILY)_LOCATION)/gen_image_bin.mk
