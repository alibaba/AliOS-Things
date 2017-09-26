NAME := mqttapp

GLOBAL_DEFINES      += ALIOT_DEBUG IOTX_DEBUG
CONFIG_OTA_CH = mqtt


$(NAME)_SOURCES     := mqttest.c

ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
PLATFORM_MQTT := linux
#NETWORK_MQTT := linuxsock
else ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
PLATFORM_MQTT := linux
#NETWORK_MQTT := linuxsock
else ifeq ($(findstring mk3060, $(BUILD_STRING)), mk3060)
PLATFORM_MQTT := rhino
#NETWORK_MQTT := rhinosock
endif

$(NAME)_SOURCES += ../../utility/iotx-utils/hal/$(PLATFORM_MQTT)/HAL_OS_$(PLATFORM_MQTT).c
$(NAME)_SOURCES += ../../utility/iotx-utils/hal/$(PLATFORM_MQTT)/HAL_TCP_$(PLATFORM_MQTT).c

$(NAME)_SOURCES += ../../utility/iotx-utils/mbedtls-lib/HAL_TLS_mbedtls.c

#$(NAME)_INCLUDES := ../../framework/protocol/alink/json/

$(NAME)_INCLUDES  += ../../utility/iotx-utils/hal \
../../utility/iotx-utils/digest \
../../utility/iotx-utils/mqtt \
../../utility/iotx-utils/guider \
../../utility/iotx-utils/LITE-log \
../../utility/iotx-utils/LITE-utils \
../../utility/iotx-utils/sdk-impl \
../../utility/iotx-utils/sdk-impl/imports \
../../utility/iotx-utils/misc
#$(NAME)_COMPONENTS := log connectivity protocol.alink.json
$(NAME)_COMPONENTS := log
$(NAME)_COMPONENTS  += mbedtls
$(NAME)_COMPONENTS  += connectivity.mqtt

LWIP := 0
ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif
