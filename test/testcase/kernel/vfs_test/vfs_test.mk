NAME := vfs_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_COMPONENTS  += kernel.fs.vfs

$(NAME)_SOURCES     += vfs_test.c

$(NAME)_CFLAGS      += -Wall -Werror

