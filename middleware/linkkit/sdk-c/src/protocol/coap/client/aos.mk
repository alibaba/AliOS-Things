NAME := libiot_coap_cloud

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := coap client

$(NAME)_SOURCES := ./iotx_coap_api.c \
                   ./CoAPNetwork.c \
                   ./CoAPMessage.c \
                   ./CoAPExport.c \

$(NAME)_COMPONENTS := libiot_utils libiot_log libiot_system libiot_sdk_impl libiot_coap_coappack iotx-hal

$(NAME)_INCLUDES := ../../../../src/infra \
                    ../../../../src/infra/log \
                    ../../../../src/infra/utils \
                    ../../../../src/infra/utils/misc \
                    ../../../../src/infra/utils/digest \
                    ../../../../src/infra/system \

