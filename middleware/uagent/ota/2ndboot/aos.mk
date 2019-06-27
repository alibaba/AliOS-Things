NAME := ota_2ndboot

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := an common 2ndboot to support device OTA upgrade, inlcude DIFF/XZ/COPY/Ymoderm upgrade.

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_SOURCES += 2ndboot.c \
                   ymodem.c

$(NAME)_SOURCES += updater/nbpatch.c \
                   updater/nbpatch_io.c \
                   updater/xzdec.c \
                   updater/libc.c  \
                   updater/xzupdater.c \
                   updater/xz/linux/lib/xz/xz_crc32.c \
                   updater/xz/linux/lib/xz/xz_dec_lzma2.c \
                   updater/xz/linux/lib/xz/xz_dec_stream.c  

$(NAME)_INCLUDES += updater/xz/linux/include/linux \
                    updater/xz/linux/lib/xz \
                    updater/xz/userspace \

GLOBAL_INCLUDES += . include updater 

CONFIG_SYSINFO_2NDBOOT_VERSION = 2ndboot-1.0.0-$(CURRENT_TIME)
$(info 2ndboot version:${CONFIG_SYSINFO_2NDBOOT_VERSION})
GLOBAL_CFLAGS += -DSYSINFO_2NDBOOT_VERSION=\"$(CONFIG_SYSINFO_2NDBOOT_VERSION)\"
