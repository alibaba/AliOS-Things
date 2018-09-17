NAME := fatfs_test

$(NAME)_COMPONENTS  += 3rdparty.experimental.fs.fatfs

$(NAME)_SOURCES     += fatfs_test.c

$(NAME)_CFLAGS      += -Wall -Werror

