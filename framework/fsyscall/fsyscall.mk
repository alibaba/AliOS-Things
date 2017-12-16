NAME := fsyscall

$(NAME)_TYPE := framework
$(NAME)_INCLUDES := .

$(NAME)_SOURCES := syscall_ftbl.c

ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_INCLUDES += ./ ./syscall_kapi/

GLOBAL_DEFINES += AOS_BINS

