NAME := libiot_http

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := A communication channel implemented in HTTP

$(NAME)_SOURCES := ./iotx_http_api.c \
 
$(NAME)_COMPONENTS := libiot_utils libiot_log libiot_system libiot_sdk_impl

$(NAME)_INCLUDES := ../../../src/infra \
../../../src/infra/log \
../../../src/infra/utils \
../../../src/infra/utils/misc \
../../../src/infra/utils/digest \
../../../src/infra/system \

