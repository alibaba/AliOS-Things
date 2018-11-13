NAME := usyscall

$(NAME)_MBINS_TYPE := app

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

$(NAME)_SOURCES := usyscall.c

GLOBAL_INCLUDES += ../include

