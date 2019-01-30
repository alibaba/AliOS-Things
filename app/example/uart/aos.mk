NAME := uart

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := this is a uart sample form hal uart.h
$(NAME)_SOURCES := uart.c

GLOBAL_DEFINES += AOS_NO_WIFI
