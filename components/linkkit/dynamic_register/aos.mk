NAME := libiot_dynreg

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.1
$(NAME)_SUMMARY := dynamic register service

$(NAME)_SOURCES := dynreg.c

$(NAME)_COMPONENTS := libiot_wrappers libiot_infra 

$(NAME)_INCLUDES += .


