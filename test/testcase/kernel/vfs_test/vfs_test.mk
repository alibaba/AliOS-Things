NAME := vfs_test

$(NAME)_COMPONENTS  += kernel.fs.vfs

$(NAME)_SOURCES     += vfs_test.c

$(NAME)_CFLAGS      += -Wall -Werror

