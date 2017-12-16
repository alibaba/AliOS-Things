NAME := fatfs_test

$(NAME)_COMPONENTS  += modules.fs.fatfs

$(NAME)_SOURCES     += fatfs_test.c

$(NAME)_CFLAGS      += -Wall -Werror

