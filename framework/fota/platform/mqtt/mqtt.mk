NAME := fota_mqtt_transport

$(NAME)_TYPE := kernel
$(NAME)_SOURCES := ota_transport.c
$(NAME)_INCLUDES := ../ \
                    ../../ \
                    ../../../mqtt/sdk-impl \
                    ../../../mqtt/platform          
