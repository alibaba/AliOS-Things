NAME := ramfs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := RAM file system

$(NAME)_SOURCES += ramfs.c ramfs_adapt.c

AOS_COMP_VFS ?= y
ifeq ($(AOS_COMP_VFS),y)
$(NAME)_COMPONENTS += kernel.fs.vfs
$(NAME)_SOURCES    += ramfs_vfs.c
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_INCLUDES += .
GLOBAL_INCLUDES += include
GLOBAL_DEFINES  += AOS_RAMFS
