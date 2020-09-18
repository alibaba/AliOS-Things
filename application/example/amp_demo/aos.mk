NAME := ampapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := amp demo
$(NAME)_SOURCES := maintask.c ampdemo.c

$(NAME)_COMPONENTS += osal_aos kv littlefs amp cli

$(NAME)_INCLUDES += ./
