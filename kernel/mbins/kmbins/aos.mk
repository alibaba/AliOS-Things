NAME := kmbins

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := mbins for kernel

$(NAME)_CFLAGS += -Wall -Werror


ifneq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := kmbins_tbl.c

$(NAME)_INCLUDES := ./
GLOBAL_INCLUDES += ./
endif

GLOBAL_DEFINES += AOS_BINS
