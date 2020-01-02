NAME := httpclient_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := HTTP client app

$(NAME)_SOURCES := httpapp.c
$(NAME)_COMPONENTS := http netmgr cli

GLOBAL_DEFINES += BUILD_AOS

CONFIG_HTTP_SECURE=1
