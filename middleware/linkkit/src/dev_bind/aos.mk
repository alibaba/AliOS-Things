NAME := libiot_bind

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.1
$(NAME)_SUMMARY := Alibaba Wireless Setup Service


$(NAME)_INCLUDES += . impl/os/  impl/awss_reset 

ifeq (y,$(strip $(AWSS_SUPPORT_AHA)))
$(NAME)_INCLUDES += ../wifi_provision/phone_ap
endif 

$(NAME)_SOURCES := impl/*.c  impl/awss_reset/*.c impl/os/*.c 

GLOBAL_INCLUDES +=  . impl/


$(NAME)_COMPONENTS := libiot_infra libiot_wrappers libiot_coap



