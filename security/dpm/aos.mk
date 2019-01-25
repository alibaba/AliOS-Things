NAME := dpm

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := device password management.

GLOBAL_INCLUDES  += ../include/dpm

$(NAME)_INCLUDES += ./inc

$(NAME)_DEFINES  += CONFIG_PLAT_AOS

ifeq ($(CONFIG_LS_DEBUG), n)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libdpm.a
else
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libdpm.a.dbg
endif

$(NAME)_COMPONENTS  := id2
$(NAME)_COMPONENTS  += alicrypto
$(NAME)_COMPONENTS  += irot
