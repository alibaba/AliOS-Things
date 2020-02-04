NAME := uagent

$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := uagent module
$(NAME)_MBINS_TYPE := kernel

$(NAME)_COMPONENTS += osal_aos

GLOBAL_DEFINES += AOS_COMP_UAGENT

$(NAME)_SOURCES     := uagent_comm_mqtt.c \
                       uagent_core.c \
                       uagent_send.c \
                       uagent_utility.c

$(NAME)_CFLAGS      += -Wall -Werror

$(NAME)_INCLUDES += include
