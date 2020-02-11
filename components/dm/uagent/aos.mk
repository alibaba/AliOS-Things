# component name
NAME := uagent
# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := uagent component
# source files and the folder of internal include files
$(NAME)_INCLUDES += include
$(NAME)_SOURCES := uagent_comm_mqtt.c \
                   uagent_core.c      \
                   uagent_send.c      \
                   uagent_utility.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS-y      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS-y      += -Wall -Werror
endif

# the folder of API files
GLOBAL_INCLUDES += ../../../include/dm/uagent
