NAME := newlib_stub

ifneq ($(HOST_MCU_FAMILY),linux)
$(NAME)_TYPE := share
$(NAME)_SOURCES := newlib_stub.c
$(NAME)_LINK_FILES := newlib_stub.o
endif
