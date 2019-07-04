NAME := libiot_dynreg

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.1
$(NAME)_SUMMARY := dynamic register service

$(NAME)_SOURCES := dynreg.c

GLOBAL_INCLUDES += .

#GLOBAL_DEFINES += 

$(NAME)_COMPONENTS := libiot_wrappers libiot_infra 




