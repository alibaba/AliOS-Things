NAME := httpapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := HTTP Client app
$(NAME)_SOURCES := httpapp.c

$(NAME)_COMPONENTS := yloop cli netmgr http mbedtls

GLOBAL_DEFINES += BUILD_AOS
