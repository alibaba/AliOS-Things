NAME := libiot_mqtt

$(NAME)_SOURCES := ./client/mqtt_client.c \
./client/mqtt_instance.c \
./MQTTPacket/MQTTPacket.c \
./MQTTPacket/MQTTSubscribeClient.c \
./MQTTPacket/MQTTDeserializePublish.c \
./MQTTPacket/MQTTUnsubscribeClient.c \
./MQTTPacket/MQTTConnectClient.c \
./MQTTPacket/MQTTSerializePublish.c \
 
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
./client \
./MQTTPacket \

