NAME := sal_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := socket adapter layer app
$(NAME)_SOURCES := sal_app.c

$(NAME)_COMPONENTS := yloop cli netmgr sal

$(NAME)_INCLUDES += ./
