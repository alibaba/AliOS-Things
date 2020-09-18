NAME := fs_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := fs demo
$(NAME)_SOURCES := app.c maintask.c

$(NAME)_COMPONENTS += osal_aos cli littlefs

$(NAME)_INCLUDES += ./
