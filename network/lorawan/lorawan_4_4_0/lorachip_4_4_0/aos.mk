NAME := lorachip_4_4_0

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := lorachip component for LoRaWAN Protocal Stack 4.4.0

ifeq ($(LORACHIP), sx1276)
$(NAME)_SOURCES := sx1276/sx1276.c
GLOBAL_INCLUDES += sx1276
endif
