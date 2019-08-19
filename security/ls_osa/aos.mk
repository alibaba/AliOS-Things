NAME := ls_osa

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := link security SDK OS abstract layer.
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR := ../include/osa

GLOBAL_INCLUDES  += ../include/osa

$(NAME)_DEFINES  += CONFIG_NET_ENABLED

$(NAME)_SOURCES  := /src/ls_osa.c

