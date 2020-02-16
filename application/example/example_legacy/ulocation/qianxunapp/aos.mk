NAME := qianxunapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.2
$(NAME)_SUMMARY := ulocation integrated QXSI service app for demo

$(NAME)_SOURCES := qxwz_example.c
$(NAME)_COMPONENTS := yloop cli ulocation netmgr

GLOBAL_DEFINES += QXWZ_ENABLED