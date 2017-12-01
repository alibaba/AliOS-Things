NAME := fsyscall

$(NAME)_TYPE := framework
$(NAME)_INCLUDES := .

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_SOURCES := syscall_ftbl.c

GLOBAL_INCLUDES += ./ ./syscall_kapi/

GLOBAL_DEFINES += AOS_BINS

