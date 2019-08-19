NAME := libkm

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.0.2
$(NAME)_SUMMARY := key management for mcu

$(NAME)_COMPONENTS := ls_hal alicrypto

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libkm.a
