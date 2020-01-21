NAME := jsengine_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := build for JSEngine firmware
$(NAME)_SOURCES := jsengine_app.c

$(NAME)_COMPONENTS += jsengine

$(NAME)_INCLUDES += ./