NAME := usyscall

$(NAME)_INCLUDES := ./ \
		../../kernel/syscall

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_SOURCES := syscall_uapi.c

GLOBAL_DEFINES += AOS_BINS
