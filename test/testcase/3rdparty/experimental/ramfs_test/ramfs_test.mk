NAME := ramfs_test

$(NAME)_COMPONENTS  += 3rdparty.experimental.ramfs

$(NAME)_SOURCES     += ramfs_test.c

$(NAME)_CFLAGS      += -Wall -Werror

