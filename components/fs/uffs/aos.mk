NAME := uffs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := Ultra-low-cost Flash File System

$(NAME)_SOURCES +=  uffs_aos.c 

ifeq ($(AOS_COMP_VFS), y)
$(NAME)_SOURCES += uffs_vfs.c
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror -Wno-strict-aliasing -Wno-uninitialized
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror -Wno-strict-aliasing -Wno-uninitialized
endif

GLOBAL_DEFINES  += AOS_COMP_UFFS
