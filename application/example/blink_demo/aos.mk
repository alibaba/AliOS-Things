NAME := blink_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := blink_demo
$(NAME)_SOURCES := blink_demo.c maintask.c

$(NAME)_COMPONENTS += osal_aos yloop

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_INCLUDES += ./
