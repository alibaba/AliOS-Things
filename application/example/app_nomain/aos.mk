NAME := app_nomain

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := app demo
$(NAME)_SOURCES := main.c app_nomain.c

$(NAME)_COMPONENTS += osal_aos

GLOBAL_DEFINES += AOS_NO_WIFI

$(NAME)_INCLUDES += ./
