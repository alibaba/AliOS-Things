NAME := ramfs_test

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := testcase for ramfs

$(NAME)_COMPONENTS += ramfs

$(NAME)_SOURCES += ramfs_test.c

$(NAME)_CFLAGS += -Wall -Werror

