NAME := ota_transport

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := ota transport message on clould.

ifeq ($(TRANSPORT),coap)
$(NAME)_SOURCES := ota_transport_coap.c
else
$(NAME)_SOURCES := ota_transport_mqtt.c 
endif

GLOBAL_INCLUDES += . ../../inc ../../hal ../verify ../verify/base64 ../verify/hash ../verify/rsa


