NAME := aos_vendor_cli

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := vendor cli command.

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall #-Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall #-Werror
endif

$(NAME)_COMPONENTS += cli

$(NAME)_SOURCES +=  besphy_functions.c \
					cli_cmd_functions.c \
					factory_functions.c \
					iwpriv_functions.c

$(NAME)_INCLUDES += ./../../drivers/services/wifi_app/audio_dec
$(NAME)_INCLUDES += ./../../drivers/services/a7_dsp/common
$(NAME)_INCLUDES += ./../../drivers/net/ftp_client

GLOBAL_INCLUDES += .
GLOBAL_DEFINES += AOS_VENDOR_CLI

