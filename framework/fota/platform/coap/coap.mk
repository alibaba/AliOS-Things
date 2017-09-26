include $(BUILD_MODULE)
NAME := coap_ota

$(NAME)_INCLUDES :=  \
    ../ \
	../../ \

$(NAME)_SOURCES := \
	ota_transport.c

$(NAME)_COMPONENTS += cjson
$(NAME)_CFLAGS := \
	$(filter-out -Werror,$(CFLAGS))

$(NAME)_DEFINES += DEBUG
GLOBAL_DEFINES      += IOTX_DEBUG
