NAME := fota_mqtt_transport

$(NAME)_TYPE := kernel
$(NAME)_SOURCES := ota_transport.c version_report.c
$(NAME)_INCLUDES := ../ \
                    ../../ \
                    ../../connectivity/mqtt/ \
                    ../../../protocol/alink-ilop/sdk-encap \
                    ../../../protocol/alink-ilop/base/utils
$(NAME)_COMPONENTS  += activation                  
