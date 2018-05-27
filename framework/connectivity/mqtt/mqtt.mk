NAME := mqtt

GLOBAL_INCLUDES += ./
$(NAME)_SOURCES += mqtt_client.c   mqtt_instance.c


ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
PLATFORM_MQTT := linux
else 
PLATFORM_MQTT := rhino
endif

GLOBAL_DEFINES += MQTT_COMM_ENABLED
$(NAME)_CFLAGS    += -DOTA_SIGNAL_CHANNEL=1 

$(NAME)_COMPONENTS := connectivity.mqtt.MQTTPacket protocol.linkkit.iotkit
ifneq ($(no_tls),1)
$(NAME)_COMPONENTS += mbedtls
endif

