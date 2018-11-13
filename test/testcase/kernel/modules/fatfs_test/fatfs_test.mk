NAME := fatfs_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_COMPONENTS  += 3rdparty.experimental.fs.fatfs

$(NAME)_SOURCES     += fatfs_test.c

$(NAME)_CFLAGS      += -Wall -Werror

