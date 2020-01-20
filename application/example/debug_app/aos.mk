NAME := debug_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := demo for debug
$(NAME)_SOURCES := debug_app.c

$(NAME)_COMPONENTS  += debug osal_aos cli kv
