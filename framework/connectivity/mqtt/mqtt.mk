NAME := mqtt

GLOBAL_INCLUDES += ./
$(NAME)_SOURCES += mqtt_client.c   mqtt_instance.c
GLOBAL_INCLUDES += ../../protocol/alink-ilop/sdk-encap ../../protocol/alink-ilop/base/utils 
$(NAME)_INCLUDES += ../../protocol/alink-ilop/iotkit-system  ../../protocol/alink-ilop/base/log/LITE-log ../../protocol/alink-ilop/base/utils/LITE-utils/src ../../protocol/alink-ilop/base/utils/misc/  ../../protocol/alink-ilop/base/utils/digest

ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
PLATFORM_MQTT := linux
else 
PLATFORM_MQTT := rhino
endif

GLOBAL_DEFINES += MQTT_COMM_ENABLED  CMP_VIA_MQTT_DIRECT MQTT_DIRECT
$(NAME)_CFLAGS    += -DOTA_SIGNAL_CHANNEL=1 

$(NAME)_COMPONENTS := connectivity.mqtt.MQTTPacket protocol.alink-ilop
ifneq ($(no_tls),1)
$(NAME)_COMPONENTS += mbedtls
endif

