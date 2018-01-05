NAME := arch_linux

$(NAME)_SOURCES:=

ifneq ($(vcall),posix)
$(NAME)_SOURCES     += cpu_impl.c
ifeq ($(PLATFORM),linuxhost)
$(NAME)_SOURCES     += swap.S
endif
endif

GLOBAL_INCLUDES += .
