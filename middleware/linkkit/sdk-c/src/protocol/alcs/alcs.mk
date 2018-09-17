NAME := libiot_alcs

$(NAME)_SOURCES := ./CoAPDeserialize.c \
./alcs_coap.c \
./CoAPResource.c \
./CoAPNetwork.c \
./CoAPPlatform.c \
./alcs_mqtt.c \
./alcs_api.c \
./CoAPObserve.c \
./CoAPServer.c \
./CoAPMessage.c \
./alcs_adapter.c \
./alcs_server.c \
./CoAPExport.c \
./alcs_client.c \
./CoAPSerialize.c \
 
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
../../../src/protocol/mqtt \
../../../src/protocol/mqtt/client \
../../../src/protocol/mqtt/MQTTPacket \

