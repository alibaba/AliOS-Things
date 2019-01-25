NAME := irot

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := root of trust, based on mcu, se or tee

GLOBAL_INCLUDES += ../include/irot

ifeq ($(CONFIG_LS_KM_SE), y)
$(NAME)_COMPONENTS := libkm_se
else ifeq ($(CONFIG_LS_KM_TEE), y)
$(NAME)_COMPONENTS := libkm_tee
else
$(NAME)_COMPONENTS := libkm
endif

