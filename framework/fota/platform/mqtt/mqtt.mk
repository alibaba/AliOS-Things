NAME := fota_mqtt_transport

$(NAME)_TYPE := kernel
$(NAME)_SOURCES := ota_transport.c
$(NAME)_INCLUDES := ../ \
                    ../../ \
                    ../../connectivity/mqtt/ \

vcall ?= rhino
ifeq ($(vcall),rhino)
GLOBAL_DEFINES += VCALL_RHINO
endif                  
ifneq (,$(filter protocol.linkkit.cm,$(COMPONENTS)))    
$(NAME)_DEFINES := WITH_CM          
endif
