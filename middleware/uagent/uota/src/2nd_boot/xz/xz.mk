NAME := ota_2boot_xz

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_SOURCES := linux/lib/xz/xz_crc32.c \
                   linux/lib/xz/xz_dec_lzma2.c \
                   linux/lib/xz/xz_dec_stream.c
                   
$(NAME)_INCLUDES := linux/include/linux \
                    linux/lib/xz \
                    userspace


