NAME := fota_mqtt_transport

$(NAME)_TYPE := kernel
$(NAME)_SOURCES := ota_transport.c version_report.c
$(NAME)_INCLUDES := ../ \
                    ../../ \
                    ../../connectivity/mqtt/ \
                    ../../../protocol/alink-ilop/sdk-encap \
                    ../../../protocol/alink-ilop/base/utils

vcall ?= rhino
ifeq ($(vcall),rhino)
GLOBAL_DEFINES += VCALL_RHINO
$(NAME)_COMPONENTS  += activation
endif                  
ifneq (,$(filter protocol.linkkit.cm,$(COMPONENTS)))    
$(NAME)_DEFINES := WITH_CM          
endif
