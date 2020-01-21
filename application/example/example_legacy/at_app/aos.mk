NAME := at_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := at app
$(NAME)_SOURCES := at_app.c

$(NAME)_COMPONENTS += at

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_INCLUDES += ./
