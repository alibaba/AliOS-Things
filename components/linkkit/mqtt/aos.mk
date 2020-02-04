NAME := libiot_mqtt

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.2
$(NAME)_SUMMARY := Implementation of MQTT protocol

$(NAME)_SOURCES := mqtt_api.c 

$(NAME)_COMPONENTS := libiot_wrappers libiot_infra libiot_sign

$(NAME)_SOURCES := mqtt_api.c 
$(NAME)_INCLUDES += .
$(NAME)_SOURCES-$(MQTT_DEFAULT_IMPL) += impl/*.c

$(NAME)_DEFINES   +=  \
    WITH_MQTT_ZIP_TOPIC=1 \
	WITH_MQTT_SUB_SHORTCUT=1 \
    WITH_MQTT_DYN_BUF=1  \
    MUTE_VERSION_REPORT

ifeq (y,$(strip $(LOG_REPORT_TO_CLOUD)))
$(NAME)_INCLUDES += ../dev_model
endif
