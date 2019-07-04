NAME := libiot_sign

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.1
$(NAME)_SUMMARY := device sign service

$(NAME)_SOURCES := dev_sign_mqtt.c 

 GLOBAL_INCLUDES += .

$(NAME)_COMPONENTS := libiot_wrappers libiot_infra
