NAME := vfs_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := testcase for vfs
$(NAME)_COMPONENTS += vfs

$(NAME)_SOURCES += vfs_test.c

$(NAME)_CFLAGS += -Wall -Werror

