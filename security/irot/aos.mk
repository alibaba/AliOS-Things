NAME := irot

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.0.0
$(NAME)_SUMMARY := root of trust, based on mcu, se or tee
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR := ../include/irot

GLOBAL_INCLUDES += ../include/irot

ifeq ($(CONFIG_LS_KM_SE), y)
$(NAME)_COMPONENTS := libkm_se
else ifeq ($(CONFIG_LS_KM_TEE), y)
$(NAME)_COMPONENTS := libkm_tee
else
$(NAME)_COMPONENTS := libkm
endif

