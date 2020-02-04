NAME := libiot_coap

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.2
$(NAME)_SUMMARY := Alibaba coap library


$(NAME)_INCLUDES += CoAPPacket

ifeq (y,$(strip $(COAP_COMM_ENABLED)))
$(NAME)_INCLUDES += client
endif
ifeq (y,$(strip $(COAP_SERVER)))
$(NAME)_INCLUDES += server
endif

$(NAME)_SOURCES := CoAPPacket/*.c  

$(NAME)_SOURCES-$(COAP_COMM_ENABLED) += client/*.c
$(NAME)_SOURCES-$(COAP_SERVER) += server/*.c

$(NAME)_INCLUDES += .


$(NAME)_COMPONENTS := libiot_infra libiot_wrappers



