NAME := libkm

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_COMPONENTS := irot.km.platform alicrypto

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libkm.a
