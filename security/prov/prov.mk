NAME := prov

DEBUG := no

GLOBAL_INCLUDES     += include

$(NAME)_CFLAGS      += -Wall -Werror -Os

$(NAME)_COMPONENTS := plat_gen alicrypto libid2 libkm

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/prov.a

