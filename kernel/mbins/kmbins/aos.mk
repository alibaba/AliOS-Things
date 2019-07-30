NAME := kmbins

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := mbins for kernel

$(NAME)_CFLAGS += -Wall -Werror


$(NAME)_SOURCES := kmbins_tbl.c

$(NAME)_INCLUDES := ./

GLOBAL_DEFINES += AOS_BINS
