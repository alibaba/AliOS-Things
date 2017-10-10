NAME := MQTTPacket

MQTTPacket_srcs := MQTTConnectClient.c \
	MQTTDeserializePublish.c \
	MQTTPacket.c \
	MQTTSerializePublish.c \
	MQTTSubscribeClient.c  \
	MQTTUnsubscribeClient.c

$(NAME)_SOURCES := $(MQTTPacket_srcs)
