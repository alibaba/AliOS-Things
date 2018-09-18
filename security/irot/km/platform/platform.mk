NAME := libplat_gen

GLOBAL_INCLUDES     += include

$(NAME)_COMPONENTS := alicrypto

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libplat_gen.a
