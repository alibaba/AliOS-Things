NAME := knxctl

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := KNX Controller Example
$(NAME)_SOURCES := knxctl.c maintask.c

$(NAME)_COMPONENTS += osal_aos cli knx

GLOBAL_DEFINES += AOS_NO_WIFI

GLOBAL_INCLUDES += ./
