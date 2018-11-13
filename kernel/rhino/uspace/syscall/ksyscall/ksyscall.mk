NAME := ksyscall

$(NAME)_MBINS_TYPE := kernel

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

$(NAME)_SOURCES := ksyscall.c
GLOBAL_INCLUDES += ../include

