NAME := libiot_mqtt

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.1
$(NAME)_SUMMARY := Implementation of MQTT protocol

$(NAME)_SOURCES := mqtt_api.c 

GLOBAL_INCLUDES += . impl

$(NAME)_COMPONENTS := libiot_wrappers libiot_infra libiot_sign

$(NAME)_SOURCES := mqtt_api.c 

$(NAME)_SOURCES-$(MQTT_DEFAULT_IMPL) += impl/*.c

$(NAME)_DEFINES   +=  \
    WITH_MQTT_ZIP_TOPIC=1 \
	WITH_MQTT_SUB_SHORTCUT=1 \
    WITH_MQTT_DYN_BUF=1

