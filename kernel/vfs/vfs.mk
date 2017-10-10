NAME := vfs

$(NAME)_TYPE 	    := kernel
$(NAME)_SOURCES     := vfs.c
$(NAME)_SOURCES     += device.c
$(NAME)_SOURCES     += vfs_inode.c
$(NAME)_SOURCES     += vfs_register.c

$(NAME)_DEFINES     += IO_NEED_TRAP
$(NAME)_CFLAGS      += -Wall -Werror
GLOBAL_INCLUDES     += include
GLOBAL_DEFINES      += AOS_VFS
