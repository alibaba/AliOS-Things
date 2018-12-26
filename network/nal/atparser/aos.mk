NAME := atparser

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := component for sending and receiving AT command over uart

GLOBAL_INCLUDES += ./

$(NAME)_SOURCES := atparser.c atparser_arch.c
GLOBAL_DEFINES += AOS_ATCMD

ifeq (y,$(FEATURE_HDLC_UART))
$(NAME)_SOURCES += hdlc.c
GLOBAL_DEFINES += HDLC_UART
endif