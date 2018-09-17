NAME := ksyscall

$(NAME)_MBINS_TYPE := kernel
$(NAME)_INCLUDES := .

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

ifeq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := ksyscall.c

GLOBAL_INCLUDES += ./ \
                   ../include
endif

GLOBAL_DEFINES +=

