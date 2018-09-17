NAME := usyscall

$(NAME)_MBINS_TYPE := app
$(NAME)_INCLUDES := .

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

ifeq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := usyscall.c

GLOBAL_INCLUDES += ./ \
                   ../include \
                   ../../uspace/include
endif

GLOBAL_DEFINES +=

