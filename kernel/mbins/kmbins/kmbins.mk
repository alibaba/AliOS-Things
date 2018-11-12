NAME := kmbins

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_COMPONENTS :=

ifneq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES := kmbins_tbl.c

$(NAME)_INCLUDES := ./
GLOBAL_INCLUDES += ./
endif

GLOBAL_DEFINES += AOS_BINS
