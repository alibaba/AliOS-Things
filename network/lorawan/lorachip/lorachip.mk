NAME := lorachip

$(NAME)_TYPE := kernel

ifeq ($(LORACHIP), sx1276)
$(NAME)_SOURCES := sx1276/sx1276.c
GLOBAL_INCLUDES += sx1276
endif
