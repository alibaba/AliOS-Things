NAME := mcu_moc108

HOST_OPENOCD   := moc108
GLOBAL_DEFINES += CONFIG_MX108

GLOBAL_CFLAGS += -mcpu=arm968e-s            \
                 -mthumb -mthumb-interwork  \
                 -mlittle-endian            \
                 -w

GLOBAL_INCLUDES += include/lwip/port        \
                   include/common           \
                   include/app/config       \
                   include/driver/include   \
                   include/driver/common    \
                   include/driver/entry     \
                   include/driver/icu       \
                   include/driver/wdt       \
                   include/ip               \
                   2ndboot

GLOBAL_LDFLAGS += -mcpu=arm968e-s           \
                  -mthumb -mthumb-interwork \
                  -mlittle-endian           \
                  --specs=nosys.specs       \
                  -nostartfiles

ifeq ($(AOS_2NDBOOT_SUPPORT), yes)
$(NAME)_COMPONENTS += bootloader
$(NAME)_LIBSUFFIX := _2ndboot

GLOBAL_LDS_FILES += platform/mcu/moc108/link/mx108_2ndboot.ld.S

$(NAME)_CFLAGS += -marm
$(NAME)_SOURCES  += 2ndboot/flash.c \
                    2ndboot/uart.c  \
                    2ndboot/sys.c   \
                    2ndboot/wdg.c   \
                    2ndboot/boot_2ndboot.S

else
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := driver & sdk for platform/mcu moc108
$(NAME)_COMPONENTS := arch_armv5
$(NAME)_COMPONENTS += newlib_stub rhino yloop
$(NAME)_COMPONENTS += lwip netmgr libprov

GLOBAL_LDFLAGS  += $(CLIB_LDFLAGS_NANO_FLOAT)


GLOBAL_LDS_FILES += platform/mcu/moc108/link/mx108.ld.S
$(NAME)_PREBUILT_LIBRARY := libmoc108.a
endif

EXTRA_TARGET_MAKEFILES +=  $($(HOST_MCU_FAMILY)_LOCATION)/gen_image_bin.mk

