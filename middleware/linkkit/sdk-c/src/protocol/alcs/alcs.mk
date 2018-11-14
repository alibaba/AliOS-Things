NAME := libiot_alcs

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY :=

$(NAME)_SOURCES := ./alcs_coap.c \
                   ./alcs_mqtt.c \
                   ./alcs_api.c \
                   ./alcs_adapter.c \
                   ./alcs_server.c \
                   ./alcs_client.c \
                   ./alcs_localsetup.c

$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
middleware/linkkit/sdk-c/src/infra/log \
middleware/linkkit/sdk-c/src/infra/system \
middleware/linkkit/sdk-c/src/sdk-impl \


$(NAME)_INCLUDES := ../../../src/infra \
                    ../../../src/infra/log \
                    ../../../src/infra/utils \
                    ../../../src/infra/utils/misc \
                    ../../../src/infra/utils/digest \
                    ../../../src/infra/system \
                    ../../../src/protocol/coap/local \
                    ../../../src/protocol/mqtt \
                    ../../../src/protocol/mqtt/client \
                    ../../../src/protocol/mqtt/MQTTPacket \

