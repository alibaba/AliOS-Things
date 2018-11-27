NAME := hal

$(NAME)_TYPE := kernel
$(NAME)_MBINS_TYPE := kernel

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_SOURCES := hal.c
GLOBAL_DEFINES      += AOS_HAL
GLOBAL_INCLUDES     += include ../rhino
