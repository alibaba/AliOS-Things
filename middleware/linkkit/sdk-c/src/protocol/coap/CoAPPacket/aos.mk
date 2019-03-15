NAME := libiot_coap_coappack

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := coap server

$(NAME)_SOURCES := ./CoAPDeserialize.c \
                   ./CoAPMessage.c \
                   ./CoAPSerialize.c \

$(NAME)_COMPONENTS := libiot_utils libiot_log libiot_system libiot_sdk_impl iotx-hal


$(NAME)_INCLUDES := ../../../../src/infra \
                    ../../../../src/infra/log \
                    ../../../../src/infra/utils \
                    ../../../../src/infra/utils/misc \
                    ../../../../src/infra/utils/digest \
                    ../../../../src/infra/system \
                    ../../../../src/protocol/mqtt \
                    ../../../../src/protocol/coap \
                    ../../../../src/protocol/mqtt/client \
                    ../../../../src/protocol/mqtt/MQTTPacket \

