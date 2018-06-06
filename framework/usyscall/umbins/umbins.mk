NAME := umbins

$(NAME)_MBINS_TYPE := app

$(NAME)_INCLUDES := ./ \
		../../../kernel/ksyscall/kmbins

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_COMPONENTS :=

$(NAME)_SOURCES := syscall_uapi.c

GLOBAL_DEFINES += AOS_BINS
