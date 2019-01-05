NAME := ota_core

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := OTA core component for transport, downlaod, verify. 

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_SOURCES := ota_service.c \
                   download/ota_download_http.c \
                   transport/ota_transport_mqtt.c \
                   verify/ota_hash.c \
                   verify/ota_sign.c

GLOBAL_INCLUDES += ../include ../hal verify
