NAME := ksyscall

$(NAME)_TYPE := kernel
$(NAME)_INCLUDES := .

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_SOURCES := syscall_ktbl.c

GLOBAL_INCLUDES += ./

GLOBAL_DEFINES += AOS_BINS

