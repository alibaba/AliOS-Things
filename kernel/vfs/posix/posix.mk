NAME := vfs_posix

$(NAME)_SOURCES     += dirent.c
$(NAME)_SOURCES     += prctl.c

$(NAME)_INCLUDES    += ../posix/include/
