NAME := ota_2nd_boot

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := 2nd boot support single/dual banker upgrade

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_SOURCES += nbpatch/nbpatch.c \
                   nbpatch/nbpatch_io.c \
                   nbpatch/xzdec.c \
                   nbpatch/flash_hal.c \
                   nbpatch/nbpatch_main.c \
                   nbpatch/rec_md5.c \
                   nbpatch/rec_verify_fw.c

$(NAME)_INCLUDES += xz/linux/include/linux \
                    xz/linux/lib/xz \
                    xz/userspace

$(NAME)_SOURCES += xz/linux/lib/xz/xz_crc32.c \
                   xz/linux/lib/xz/xz_dec_lzma2.c \
                   xz/linux/lib/xz/xz_dec_stream.c

$(NAME)_SOURCES += rec_libc.c \
                   rec_main.c \
                   rec_2boot.c \
                   rec_ymodem.c

$(NAME)_INCLUDES += nbpatch

GLOBAL_INCLUDES += .

CONFIG_SYSINFO_2BOOT_VERSION = aos-2nd_boot-1.0.0
$(info 2boot_version:${CONFIG_SYSINFO_2BOOT_VERSION})
GLOBAL_CFLAGS += -DSYSINFO_2BOOT_VERSION=\"$(CONFIG_SYSINFO_2BOOT_VERSION)\"
