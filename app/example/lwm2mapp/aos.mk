NAME := lwm2mapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := lwm2m app
$(NAME)_SOURCES := lwm2m_example.c

$(NAME)_COMPONENTS += osal_aos lwm2m cli netmgr

GLOBAL_DEFINES += AOS_NO_WIFI

GLOBAL_INCLUDES += ./
