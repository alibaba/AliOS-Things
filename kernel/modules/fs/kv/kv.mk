NAME := kv

$(NAME)_TYPE        := kernel
$(NAME)_SOURCES     := kvmgr.c
$(NAME)_COMPONENTS  += log

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_INCLUDES     += include
GLOBAL_DEFINES      += AOS_KV
