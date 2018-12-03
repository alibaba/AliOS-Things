NAME := linkwan

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := linkwan
GLOBAL_INCLUDES += .

linkwan := 1

$(NAME)_SOURCES := linkwan.c

ifeq ($(lorawanback), 0)
$(NAME)_COMPONENTS := network.lorawan.lorawan_4_4_2
else ifeq ($(lorawanback), 1)
$(NAME)_COMPONENTS := network.lorawan.lorawan_4_4_0
endif

$(NAME)_COMPONENTS += kernel.fs.kv

GLOBAL_INCLUDES += ./
