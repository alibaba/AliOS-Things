NAME := libiot_http2

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := Nghttp2 and its encapsulation

$(NAME)_SOURCES := ./nghttp2/*.c \
                   ./iotx_http2_api.c \
 
$(NAME)_COMPONENTS := libiot_utils libiot_log libiot_system libiot_sdk_impl iotx-hal 

$(NAME)_INCLUDES := ../../../src/infra/utils \
../../../src/infra/utils/misc \
../../../src/infra/utils/digest \
../../../src/infra/log \
../../../src/protocol/http2 \
../../../src/protocol/http2/nghttp2 \
./


