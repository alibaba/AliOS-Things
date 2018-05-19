NAME := ramfs

$(NAME)_TYPE        := kernel
$(NAME)_SOURCES     += ramfs.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_INCLUDES     += .
GLOBAL_INCLUDES     += include
GLOBAL_DEFINES      += AOS_RAMFS
