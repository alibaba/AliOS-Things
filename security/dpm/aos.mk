NAME := dpm

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := device password management.
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR := ../include/dpm

GLOBAL_INCLUDES  += ../include/dpm

$(NAME)_INCLUDES += ./inc

$(NAME)_DEFINES  += CONFIG_PLAT_AOS

ifeq ($(CONFIG_LS_DEBUG), n)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libdpm.a
else
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libdpm_dbg.a
endif

$(NAME)_COMPONENTS  := id2
$(NAME)_COMPONENTS  += alicrypto
$(NAME)_COMPONENTS  += irot
