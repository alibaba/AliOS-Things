NAME := kv

$(NAME)_MBINS_TYPE := kernel
$(NAME)_SOURCES    := kv.c kv_adapt.c kv_cli.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

GLOBAL_INCLUDES += . include
GLOBAL_DEFINES  += AOS_KV
