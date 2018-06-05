NAME := ldapp

$(NAME)_SOURCES    := mqtt-example.c
$(NAME)_COMPONENTS := cli sensor connectivity.mqtt cjson fota netmgr framework.common
GLOBAL_DEFINES     += MQTT_TEST ALIOT_DEBUG IOTX_DEBUG USE_LPTHREAD
