NAME := libiot_http

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.1
$(NAME)_SUMMARY := http service

$(NAME)_SOURCES := iotx_http_api.c 

$(NAME)_COMPONENTS := libiot_wrappers libiot_infra 

$(NAME)_INCLUDES += .


