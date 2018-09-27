##
 # Copyright (C) 2015-2017 Alibaba Group Holding Limited
##

NAME := atmqttclient

GLOBAL_INCLUDES += ./

$(NAME)_SOURCES := at_mqtt_client.c

ifeq (1,$(atmqttica))
$(NAME)_SOURCES += at_mqtt_ica_client.c
endif

