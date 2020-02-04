NAME := ramfs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := RAM file system

$(NAME)_SOURCES += ramfs.c ramfs_adapt.c

ifeq ($(AOS_COMP_VFS),y)
$(NAME)_SOURCES    += ramfs_vfs.c
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif

$(NAME)_INCLUDES += include
GLOBAL_DEFINES  += AOS_COMP_RAMFS
RPM_INCLUDE_DIR := fs
