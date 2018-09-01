NAME := vfs_posix

$(NAME)_SOURCES     += dirent.c
$(NAME)_SOURCES     += prctl.c

GLOBAL_INCLUDES     += include
