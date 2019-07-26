NAME := libiot_devmodel

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.1
$(NAME)_SUMMARY := device model service

$(NAME)_INCLUDES +=  client .

ifeq (y,$(strip $(DEPRECATED_LINKKIT)))
$(NAME)_INCLUDES += deprecated
endif

$(NAME)_INCLUDES += .
ifeq (y,$(strip $(ALCS_ENABLED)))
$(NAME)_INCLUDES += server alcs
$(NAME)_INCLUDES += ../iot_coap  ../iot_coap/server
endif

$(NAME)_SOURCES := *.c  client/*.c

$(NAME)_SOURCES-$(DEPRECATED_LINKKIT) += deprecated/*.c
$(NAME)_SOURCES-$(ALCS_ENABLED) += server/*.c  alcs/*.c

$(NAME)_COMPONENTS := libiot_infra libiot_wrappers libiot_mqtt 
$(NAME)_COMPONENTS-$(ALCS_ENABLED) += libiot_coap


ifeq (y,$(strip $(DEVICE_MODEL_GATEWAY)))
        GLOBAL_CFLAGS += -DCONFIG_SDK_THREAD_COST=1
endif

