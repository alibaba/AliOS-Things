NAME := libsst

$(NAME)_CFLAGS      += -Wall -Werror -Os

GLOBAL_INCLUDES     += include

$(NAME)_COMPONENTS :=  libkm plat_gen alicrypto

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libsst.a
