NAME := libiot_ota

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.2
$(NAME)_SUMMARY := ota service

$(NAME)_SOURCES := *.c 

$(NAME)_INCLUDES += .
$(NAME)_COMPONENTS := libiot_wrappers libiot_infra 
