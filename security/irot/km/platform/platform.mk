NAME := libplat_gen

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
GLOBAL_INCLUDES     += include

$(NAME)_COMPONENTS := alicrypto

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libplat_gen.a
