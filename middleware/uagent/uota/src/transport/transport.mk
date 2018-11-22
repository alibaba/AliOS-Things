NAME := ota_transport

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_SOURCES := ota_transport_mqtt.c ota_transport_coap.c

GLOBAL_INCLUDES += . ../../inc ../../hal ../verify ../verify/base64 ../verify/hash ../verify/rsa


