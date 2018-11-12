NAME := ota_2boot_xz

$(NAME)_TYPE := uota

$(NAME)_SOURCES := linux/lib/xz/xz_crc32.c \
                   linux/lib/xz/xz_dec_lzma2.c \
                   linux/lib/xz/xz_dec_stream.c
                   
$(NAME)_INCLUDES := linux/include/linux \
                    linux/lib/xz \
                    userspace


