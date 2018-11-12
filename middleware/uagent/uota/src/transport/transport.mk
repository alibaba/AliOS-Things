NAME := ota_transport

$(NAME)_TYPE := uota

$(NAME)_SOURCES := ota_transport_mqtt.c ota_transport_coap.c

GLOBAL_INCLUDES += . ../../inc ../../hal ../verify ../verify/base64 ../verify/hash ../verify/rsa


