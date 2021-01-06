NAME := tcp_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := tcp demo
$(NAME)_SOURCES := appdemo.c maintask.c tcp_client.c

$(NAME)_COMPONENTS += osal_aos cli netmgr

$(NAME)_INCLUDES += ./
