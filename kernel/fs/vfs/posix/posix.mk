NAME := vfs_posix

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_SOURCES     += dirent.c
$(NAME)_SOURCES     += prctl.c

GLOBAL_INCLUDES     += include
