NAME := usyscall

$(NAME)_INCLUDES := ./

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_SOURCES := syscall_uapi.c

