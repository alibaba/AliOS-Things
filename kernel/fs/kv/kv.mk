NAME := kv

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    += 0.0.1.0
$(NAME)_SUMMARY    += lightweight key-value style storage module

$(NAME)_SOURCES := kv.c kv_adapt.c kv_cli.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

GLOBAL_INCLUDES += . include
GLOBAL_DEFINES  += AOS_KV

AOS_KV ?= 1
