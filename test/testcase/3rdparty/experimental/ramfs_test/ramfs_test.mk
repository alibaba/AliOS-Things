NAME := ramfs_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := testcase for ramfs

$(NAME)_COMPONENTS += kernel.fs.ramfs

$(NAME)_SOURCES += ramfs_test.c

$(NAME)_CFLAGS += -Wall -Werror

