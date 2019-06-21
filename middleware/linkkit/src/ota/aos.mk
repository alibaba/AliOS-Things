NAME := libiot_ota

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.1
$(NAME)_SUMMARY := ota service

$(NAME)_SOURCES := *.c 

GLOBAL_INCLUDES += .

#GLOBAL_DEFINES += 

$(NAME)_COMPONENTS := libiot_wrappers libiot_infra 




