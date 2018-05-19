
NAME := alcs
$(NAME)_TYPE := framework
GLOBAL_INCLUDES +=  ./ ./inc

$(NAME)_SOURCES     :=  src/alcs_adapter.c \
			src/alcs_mqtt.c \
			src/alcs_api.c \
			src/alcs_client.c \
			src/alcs_coap.c \
			src/alcs_server.c \
			src/linked_list.c

$(NAME)_COMPONENTS += framework.common connectivity.link-coap

