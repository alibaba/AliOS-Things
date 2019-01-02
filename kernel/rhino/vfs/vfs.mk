NAME := vfs

$(NAME)_TYPE 	    := kernel
$(NAME)_MBINS_TYPE  := kernel

$(NAME)_SOURCES     := vfs.c
$(NAME)_SOURCES     += vfs_file.c
$(NAME)_SOURCES     += vfs_inode.c
$(NAME)_SOURCES     += vfs_register.c

ifeq ($(HOST_ARCH),linux)
$(NAME)_DEFINES     += IO_NEED_TRAP
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES      += __BSD_VISIBLE
else ifeq ($(COMPILER),rvct)
GLOBAL_DEFINES      += __BSD_VISIBLE
endif

GLOBAL_INCLUDES     += include
GLOBAL_DEFINES      += AOS_VFS

$(NAME)_COMPONENTS  += kernel.rhino.vfs.device
