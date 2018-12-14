NAME := helloworld

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Hello World
$(NAME)_SOURCES := helloworld.c

GLOBAL_DEFINES += AOS_NO_WIFI

GLOBAL_INCLUDES += ./
