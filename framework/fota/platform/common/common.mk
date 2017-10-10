NAME := common_transport

$(NAME)_SOURCES := ota_transport.c
GLOBAL_INCLUDES += ./
$(NAME)_INCLUDES := ../../ \
					../
