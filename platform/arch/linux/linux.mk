NAME := arch_linux

$(NAME)_SOURCES:=

ifneq ($(vcall),posix)
$(NAME)_SOURCES     += cpu_impl.c
endif

GLOBAL_INCLUDES += .
