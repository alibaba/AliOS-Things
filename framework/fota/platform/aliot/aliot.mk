NAME := aliot_transport

$(NAME)_SOURCES := ota_transport.c
GLOBAL_INCLUDES += ./
$(NAME)_INCLUDES := ../ \
                    ../../ \
                    ../../../mqtt/sdk-impl \
                    ../../../mqtt/platform          
