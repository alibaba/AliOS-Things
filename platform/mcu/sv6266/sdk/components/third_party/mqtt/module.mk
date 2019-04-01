LIB_SRC := MQTTPacket/MQTTConnectClient.c
LIB_SRC += MQTTPacket/MQTTConnectServer.c
LIB_SRC += MQTTPacket/MQTTDeserializePublish.c
LIB_SRC += MQTTPacket/MQTTFormat.c
LIB_SRC += MQTTPacket/MQTTPacket.c
LIB_SRC += MQTTPacket/MQTTSerializePublish.c
LIB_SRC += MQTTPacket/MQTTSubscribeClient.c
LIB_SRC += MQTTPacket/MQTTSubscribeServer.c
LIB_SRC += MQTTPacket/MQTTUnsubscribeClient.c
LIB_SRC += MQTTPacket/MQTTUnsubscribeServer.c
LIB_SRC += MQTTClient.c
LIB_SRC += MQTTFreeRTOS.c
LIB_SRC += mqtt_test.c

LIB_ASRC :=
LIBRARY_NAME := mqtt
LOCAL_CFLAGS += -Wno-address
LOCAL_AFLAGS +=
LOCAL_INC += -I$(TOPDIR)/components/third_party/mqtt
LOCAL_INC += -I$(TOPDIR)/components/third_party/mqtt/MQTTPacket
RELEASE_SRC := 2
$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
