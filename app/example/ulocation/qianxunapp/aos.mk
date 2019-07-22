NAME := qianxunapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := ulocation integrated qianxun SDK app for demo

$(NAME)_SOURCES := qxwz_example.c

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_COMPONENTS := yloop cli ulocation netmgr

GLOBAL_INCLUDES += ./