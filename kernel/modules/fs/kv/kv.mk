NAME := kv

$(NAME)_TYPE        := kernel
$(NAME)_SOURCES     := kvmgr.c
$(NAME)_COMPONENTS  += log
$(NAME)_CFLAGS      += -Wall -Werror
GLOBAL_INCLUDES     += include
GLOBAL_DEFINES      += AOS_KV
