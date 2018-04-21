NAME := sst_test

$(NAME)_COMPONENTS  += libsst libid2 libkm plat_gen alicrypto

$(NAME)_SOURCES     += sst_test.c

$(NAME)_CFLAGS      += -Wall -Werror -Wno-pointer-sign

