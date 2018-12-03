NAME := libprov

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.0.1
$(NAME)_SUMMARY := provisioning SDK for ID2

GLOBAL_INCLUDES += ../include/prov

$(NAME)_COMPONENTS := alicrypto
$(NAME)_COMPONENTS += irot

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libprov.a

