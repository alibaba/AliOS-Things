NAME := libiot_mqtt

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := Implementation of MQTT protocol

$(NAME)_SOURCES := ./client/mqtt_client.c \
./MQTTPacket/MQTTPacket.c \
./MQTTPacket/MQTTSubscribeClient.c \
./MQTTPacket/MQTTDeserializePublish.c \
./MQTTPacket/MQTTUnsubscribeClient.c \
./MQTTPacket/MQTTConnectClient.c \
./MQTTPacket/MQTTSerializePublish.c \
 
$(NAME)_COMPONENTS := libiot_utils libiot_log libiot_system libiot_sdk_impl iotx-hal 


$(NAME)_INCLUDES := ../../../src/infra \
../../../src/infra/log \
../../../src/infra/utils \
../../../src/infra/utils/misc \
../../../src/infra/utils/digest \
../../../src/infra/system \
./client \
./MQTTPacket \

