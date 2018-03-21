NAME := xz

$(NAME)_TYPE := kernel
$(NAME)_SOURCES := decompress.c \
                   xz_crc32.c \
                   xz_dec_lzma2.c \
                   xz_dec_stream.c
