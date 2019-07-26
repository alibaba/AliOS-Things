NAME := libiot_reset

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.1
$(NAME)_SUMMARY := device reset service

$(NAME)_SOURCES := dev_reset.c 

$(NAME)_COMPONENTS := libiot_wrappers libiot_mqtt

$(NAME)_INCLUDES += .


