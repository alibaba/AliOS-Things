NAME := ota_2boot_xz

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := XZ uncompress algorithm

$(NAME)_SOURCES := linux/lib/xz/xz_crc32.c \
                   linux/lib/xz/xz_dec_lzma2.c \
                   linux/lib/xz/xz_dec_stream.c
                   
$(NAME)_INCLUDES := linux/include/linux \
                    linux/lib/xz \
                    userspace


