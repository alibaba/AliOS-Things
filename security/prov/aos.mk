NAME := libprov

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.0.1
$(NAME)_SUMMARY := provisioning SDK for ID2
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR := ../include/prov

GLOBAL_INCLUDES += ../include/prov

$(NAME)_COMPONENTS := alicrypto
$(NAME)_COMPONENTS += irot

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libprov.a

