NAME := ramfs_test

$(NAME)_COMPONENTS  += kernel.rhino.fs.ramfs

$(NAME)_SOURCES     += ramfs_test.c

$(NAME)_CFLAGS      += -Wall -Werror

