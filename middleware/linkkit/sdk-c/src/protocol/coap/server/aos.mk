NAME := libiot_coap_local

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := coap server

$(NAME)_SOURCES := ./CoAPResource.c \
                   ./CoAPNetwork.c \
                   ./CoAPPlatform.c \
                   ./CoAPObserve.c \
                   ./CoAPServer.c \
                   ./CoAPMessage.c \
                   ./CoAPExport.c \

$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
                      middleware/linkkit/sdk-c/src/infra/log \
                      middleware/linkkit/sdk-c/src/infra/system \
                      middleware/linkkit/sdk-c/src/sdk-impl \
                      middleware/linkkit/sdk-c/src/protocol/coap/CoAPPacket \


$(NAME)_INCLUDES := ../../../../src/infra \
                    ../../../../src/infra/log \
                    ../../../../src/infra/utils \
                    ../../../../src/infra/utils/misc \
                    ../../../../src/infra/utils/digest \
                    ../../../../src/infra/system \
                    ../../../../src/protocol/mqtt \
                    ../../../../src/protocol/mqtt/client \
                    ../../../../src/protocol/mqtt/MQTTPacket \

