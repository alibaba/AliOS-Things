NAME := tcp_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := tcp client example

$(NAME)_SOURCES     := tcp_client.c

$(NAME)_COMPONENTS  := cli netmgr

GLOBAL_INCLUDES += ./
