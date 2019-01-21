NAME := libiot_cm

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := Connection management

$(NAME)_SOURCES := ./iotx_cm.c 

 
 ifeq (y,$(COAP_COMM_ENABLED))
$(NAME)_SOURCES += iotx_cm_coap.c 
endif

 ifeq (y,$(MQTT_COMM_ENABLED))
$(NAME)_SOURCES += iotx_cm_mqtt.c  
endif

ifeq (y,$(MAL_ENABLED))
$(NAME)_SOURCES += iotx_cm_mqtt.c  
endif

$(NAME)_COMPONENTS := libiot_utils libiot_log

$(NAME)_INCLUDES := ../../../../src/infra \
../../../../src/infra/log \
../../../../src/infra/utils \
../../../../src/infra/utils/misc \
../../../../src/infra/utils/digest \
../../../../src/infra/system \
../../../../src/protocol/mqtt \
../../../../src/protocol/mqtt/client \
../../../../src/protocol/mqtt/MQTTPacket \
./example \
./include

