NAME := zlib

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := zlib is a general purpose data compression library.

$(NAME)_SOURCES :=  src/adler32.c \
                    src/compress.c \
                    src/crc32.c \
                    src/deflate.c \
                    src/gzclose.c \
                    src/gzlib.c \
                    src/gzread.c \
                    src/gzwrite.c \
                    src/infback.c \
                    src/inffast.c \
                    src/inflate.c \
                    src/inftrees.c \
                    src/trees.c \
                    src/uncompr.c \
                    src/zutil.c
$(NAME)_INCLUDES    += include
GLOBAL_INCLUDES     += . 
