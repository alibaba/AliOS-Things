NAME := libiot_cm

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY :=

$(NAME)_SOURCES := ./iotx_cm.c 

 
 ifeq (y,$(FEATURE_COAP_COMM_ENABLED))
$(NAME)_SOURCES += iotx_cm_coap.c 
endif

 ifeq (y,$(FEATURE_MQTT_COMM_ENABLED))
$(NAME)_SOURCES += iotx_cm_mqtt.c  
endif

ifeq (y,$(FEATURE_MAL_ENABLED))
$(NAME)_SOURCES += iotx_cm_mqtt.c  
endif

$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
middleware/linkkit/sdk-c/src/infra/log \
middleware/linkkit/sdk-c/src/infra/system \
middleware/linkkit/sdk-c/src/sdk-impl \

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

