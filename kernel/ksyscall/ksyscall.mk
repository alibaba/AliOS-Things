NAME := ksyscall

$(NAME)_TYPE := kernel
$(NAME)_INCLUDES := .

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_SOURCES := syscall_ktbl.c

GLOBAL_INCLUDES += ./

GLOBAL_DEFINES += AOS_BINS

