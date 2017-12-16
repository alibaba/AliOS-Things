NAME := tfs_test

$(NAME)_COMPONENTS  += tfs libid2 libkm plat_gen base64 digest_algorithm

$(NAME)_SOURCES     += tfs_test.c

$(NAME)_CFLAGS      += -Wall -Werror -Wno-pointer-sign

