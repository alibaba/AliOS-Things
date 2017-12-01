include $(BUILD_MODULE)
NAME := fota_coap_transport

$(NAME)_TYPE := kernel
$(NAME)_INCLUDES := ../ \
	../../ \

$(NAME)_SOURCES := \
	ota_transport.c

$(NAME)_COMPONENTS += cjson
GLOBAL_DEFINES     += IOTX_DEBUG
