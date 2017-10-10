NAME := syscall

$(NAME)_TYPE := kernel
$(NAME)_INCLUDES := .

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_SOURCES := syscall_tbl.c

GLOBAL_INCLUDES += ./

GLOBAL_DEFINES += AOS_BINS

