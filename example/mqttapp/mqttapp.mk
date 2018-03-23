
NAME := mqttapp

GLOBAL_DEFINES      += ALIOT_DEBUG IOTX_DEBUG
CONFIG_OTA_CH = mqtt
ifeq ($(STM32_NONSTD_SOCKET), true)
$(NAME)_SOURCES     := mqtt-example-b_l475e.c
$(NAME)_DEFINES     += SENSOR
else
$(NAME)_SOURCES     := mqtt-example.c
endif

$(NAME)_COMPONENTS := cli connectivity.mqtt cjson  netmgr framework.common

LWIP := 0
ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

ifeq ($(no_tls),1)
GLOBAL_DEFINES += IOTX_WITHOUT_TLS  MQTT_DIRECT
endif