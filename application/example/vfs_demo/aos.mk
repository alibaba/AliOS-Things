NAME := vfs_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := vfs demo based on ramfs

$(NAME)_SOURCES += maintask.c vfs.c

$(NAME)_COMPONENTS += osal_aos ramfs

$(NAME)_CFLAGS += -Wall -Werror
