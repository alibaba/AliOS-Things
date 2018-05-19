NAME := kmbins

$(NAME)_MBINS_TYPE := kernel
$(NAME)_INCLUDES := ./

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_COMPONENTS :=

$(NAME)_SOURCES := syscall_tbl.c

GLOBAL_INCLUDES += ./

GLOBAL_DEFINES += AOS_BINS
