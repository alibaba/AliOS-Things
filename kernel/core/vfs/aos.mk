NAME := vfs

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION    := 1.0.0
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
$(NAME)_CFLAGS += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES += __BSD_VISIBLE
else ifeq ($(COMPILER),rvct)
GLOBAL_DEFINES += __BSD_VISIBLE
endif

$(NAME)_INCLUDES += . include

ifeq ($(LFS_CONFIG_IN_KERNEL),y)
lsopen ?= 1
else
lsopen ?= 0
endif

ifeq ($(lsopen),1)
GLOBAL_DEFINES += CONFIG_VFS_LSOPEN
endif
