NAME := libprov

DEBUG := no

ifneq (,$(BINS))
ifeq ($(MBEDTLS_SHARE),1)
$(NAME)_TYPE := framework&kernel
else
$(NAME)_TYPE := kernel
endif
endif

GLOBAL_INCLUDES     += include

$(NAME)_COMPONENTS := alicrypto irot

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libprov.a

