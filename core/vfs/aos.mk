NAME := vfs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := Virtual File System

$(NAME)_SOURCES := vfs.c
$(NAME)_SOURCES += vfs_file.c
$(NAME)_SOURCES += vfs_inode.c
$(NAME)_SOURCES += vfs_adapt.c

ifeq ($(HOST_ARCH),linux)
$(NAME)_DEFINES += IO_NEED_TRAP
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES += __BSD_VISIBLE
else ifeq ($(COMPILER),rvct)
GLOBAL_DEFINES += __BSD_VISIBLE
endif

GLOBAL_INCLUDES += . include
GLOBAL_DEFINES  += AOS_COMP_VFS

AOS_COMP_VFS ?= y
RPM_INCLUDE_DIR := fs
