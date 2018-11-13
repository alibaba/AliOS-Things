NAME := libprov

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

DEBUG := no

GLOBAL_INCLUDES     += include

$(NAME)_COMPONENTS := alicrypto irot

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libprov.a

