NAME := ls_osa

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := link security SDK OS abstract layer.

GLOBAL_INCLUDES  += ../include/osa

$(NAME)_DEFINES  += CONFIG_NET_ENABLED

$(NAME)_SOURCES  := /src/ls_osa.c

