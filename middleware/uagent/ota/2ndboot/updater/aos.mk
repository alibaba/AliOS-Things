NAME := ota_updater

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := OTA updater for DIFF/XZ upgrade.

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/ota_updater.a

#$(NAME)_SOURCES += nbpatch.c \
                   nbpatch_io.c \
                   xzdec.c \
                   libc.c  \
                   xzupdater.c \
                   xz/linux/lib/xz/xz_crc32.c \
                   xz/linux/lib/xz/xz_dec_lzma2.c \
                   xz/linux/lib/xz/xz_dec_stream.c

#$(NAME)_INCLUDES += xz/linux/include/linux \
                    xz/linux/lib/xz \
                    xz/userspace

GLOBAL_INCLUDES += include
