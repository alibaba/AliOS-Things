NAME := mcu_moc108

HOST_OPENOCD   := moc108
GLOBAL_DEFINES += CONFIG_MX108
GLOBAL_DEFINES += CONFIG_AOS_UOTA_BREAKPOINT

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

ifeq ($(AOS_2BOOT_SUPPORT), yes)
$(NAME)_COMPONENTS += ota_2nd_boot
$(NAME)_LIBSUFFIX := _2boot

GLOBAL_2BOOT_LDS_FILES += platform/mcu/moc108/link/mx108_2boot.ld.S

$(NAME)_SOURCES  += 2ndboot/rec_flash.c \
                    2ndboot/rec_uart.c  \
                    2ndboot/rec_sys.c   \
                    2ndboot/rec_wdt.c   \
                    2ndboot/boot_recovery.S
else
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := driver & sdk for platform/mcu moc108
$(NAME)_COMPONENTS := arch_armv5
$(NAME)_COMPONENTS += newlib_stub rhino yloop alicrypto
$(NAME)_COMPONENTS += libprov
$(NAME)_COMPONENTS += lwip netmgr

GLOBAL_LDFLAGS  += $(CLIB_LDFLAGS_NANO_FLOAT)


GLOBAL_LDS_FILES += platform/mcu/moc108/link/mx108.ld.S
$(NAME)_PREBUILT_LIBRARY := libmoc108.a
endif


