NAME := prov

DEBUG := no

ifneq (,$(BINS))
ifeq ($(MBEDTLS_SHARE),1)
$(NAME)_TYPE := framework&kernel
else
$(NAME)_TYPE := kernel
endif
endif

GLOBAL_INCLUDES     += include

$(NAME)_CFLAGS      += -Wall -Werror -Os

$(NAME)_COMPONENTS := plat_gen alicrypto libid2 libkm

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libprov.a

