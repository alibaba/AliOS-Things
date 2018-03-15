NAME := uffs

$(NAME)_TYPE        := kernel

$(NAME)_SOURCES     +=  uffs_aos.c uffs_port.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror -Wno-strict-aliasing -Wno-uninitialized
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror -Wno-strict-aliasing -Wno-uninitialized
endif

GLOBAL_INCLUDES     += include
GLOBAL_DEFINES      += AOS_UFFS
