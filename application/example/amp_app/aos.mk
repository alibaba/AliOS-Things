NAME := amp_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := amp app
$(NAME)_SOURCES := maintask.c amp_app.c

$(NAME)_COMPONENTS += osal_aos kv littlefs amp cli

$(NAME)_INCLUDES += ./
