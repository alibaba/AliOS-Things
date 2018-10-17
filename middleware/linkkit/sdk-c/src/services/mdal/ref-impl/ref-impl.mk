NAME := mdal-ref-impl

$(NAME)_SOURCES := mdal_at_mqtt_client.c mdal_at_ica_mqtt_client.c 

ifeq (1,$(atmqttica))
$(NAME)_SOURCES += mdal_at_client.c
GLOBAL_DEFINES += AT_ICA_MQTT_CLIENT_SUPPORT
endif


GLOBAL_INCLUDES += ./
