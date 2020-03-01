NAME := linkkit_sdk_c

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.2
$(NAME)_SUMMARY := IoT Connectivity Devkit Running on Embedded Devices for Alibaba Cloud

CONFIG_LIB_EXPORT ?= static
RPM_INCLUDE_DIR := linkkit

#####################################################################
# Configs for component middleware.linkkit.sdk-
$(NAME)_COMPONENTS-y := libiot_wrappers
$(NAME)_COMPONENTS-$(COAP_COMM_ENABLED) += libiot_coap
$(NAME)_COMPONENTS-$(COAP_SERVER) += libiot_coap
$(NAME)_COMPONENTS-$(AOS_COMP_SDK_DEVMODEL) += libiot_devmodel
$(NAME)_COMPONENTS-$(AOS_COMP_SDK_SIGN) += libiot_sign
$(NAME)_COMPONENTS-$(DYNAMIC_REGISTER) += libiot_dynreg
$(NAME)_COMPONENTS-$(AOS_COMP_SDK_HTTP) += libiot_http
$(NAME)_COMPONENTS-$(AOS_COMP_SDK_HTTP2) += libiot_http2stream
$(NAME)_COMPONENTS-$(AOS_COMP_SDK_MQTT) += libiot_mqtt
$(NAME)_COMPONENTS-$(AOS_COMP_SDK_AWSS) += libiot_awss
$(NAME)_COMPONENTS-$(AOS_COMP_SDK_RESET) += libiot_reset
