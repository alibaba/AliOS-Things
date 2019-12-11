NAME := atparser_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := atparser app
$(NAME)_SOURCES := atparser_app.c

$(NAME)_COMPONENTS += atparser

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_INCLUDES += ./
