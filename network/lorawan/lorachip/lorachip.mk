NAME := lorachip

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

ifeq ($(LORACHIP), sx1276)
$(NAME)_SOURCES := sx1276/sx1276.c
GLOBAL_INCLUDES += sx1276
endif
