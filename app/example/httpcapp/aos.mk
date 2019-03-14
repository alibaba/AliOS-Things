NAME := httpcapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := HTTP Client app

$(NAME)_SOURCES := main.c
$(NAME)_COMPONENTS := httpc netmgr cli

GLOBAL_DEFINES += BUILD_AOS
