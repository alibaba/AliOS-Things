NAME := http

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := http client component

GLOBAL_INCLUDES += include
GLOBAL_DEFINES += CONFIG_AOS_HTTP

$(NAME)_SOURCES := src/http_client.c \
                   src/http_string.c \
                   src/http_parser.c

include $($(NAME)_LOCATION)/wrappers/http_wrapper.mk
#$(NAME)_COMPONENTS += http_wrapper
