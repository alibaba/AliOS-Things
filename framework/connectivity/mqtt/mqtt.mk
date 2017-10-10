NAME := mqtt

$(NAME)_COMPONENTS := connectivity.mqtt.MQTTPacket
$(NAME)_COMPONENTS += mbedtls
$(NAME)_COMPONENTS += connectivity.mqtt
$(NAME)_COMPONENTS += digest_algorithm
$(NAME)_COMPONENTS += iotx-utils.LITE-log iotx-utils.LITE-utils iotx-utils.misc iotx-utils.sdk-impl iotx-utils.guider iotx-utils.digest

MQTT_UTILS_PATH :=  ../../../utility/iotx-utils
#$(NAME)_INCLUDES := $(MQTT_UTILS_PATH)/digest \
					$(MQTT_UTILS_PATH)/guider \
					$(MQTT_UTILS_PATH)/hal \
					$(MQTT_UTILS_PATH)/LITE-log \
					$(MQTT_UTILS_PATH)/LITE-utils \
					$(MQTT_UTILS_PATH)/misc \
					$(MQTT_UTILS_PATH)/sdk-impl \
					$(MQTT_UTILS_PATH)/device

$(NAME)_SOURCES := mqtt_client.c

ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
PLATFORM_MQTT := linux
#NETWORK_MQTT := linuxsock
else ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
PLATFORM_MQTT := linux
#NETWORK_MQTT := linuxsock
else ifeq ($(findstring mk3060, $(BUILD_STRING)), mk3060)
PLATFORM_MQTT := rhino
#NETWORK_MQTT := rhinosock
else ifeq ($(findstring b_l475e, $(BUILD_STRING)), b_l475e)
PLATFORM_MQTT := rhino
#NETWORK_MQTT := rhinosock
endif


$(NAME)_SOURCES += $(MQTT_UTILS_PATH)/hal/$(PLATFORM_MQTT)/HAL_OS_$(PLATFORM_MQTT).c
$(NAME)_SOURCES += $(MQTT_UTILS_PATH)/hal/$(PLATFORM_MQTT)/HAL_TCP_$(PLATFORM_MQTT).c

$(NAME)_SOURCES += $(MQTT_UTILS_PATH)/mbedtls-hal/HAL_TLS_mbedtls.c
