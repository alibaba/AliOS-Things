NAME := uspace

$(NAME)_MBINS_TYPE := kernel
$(NAME)_INCLUDES := .

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_SOURCES := u_task.c

GLOBAL_INCLUDES += ./include

GLOBAL_DEFINES +=

