NAME := httpc

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := http client component

GLOBAL_INCLUDES += include
GLOBAL_DEFINES += CONFIG_AOS_HTTPC


$(NAME)_SOURCES := src/http_client.c \
                   src/http_string.c

include $($(NAME)_LOCATION)/wrappers/httpc_wrapper.mk
