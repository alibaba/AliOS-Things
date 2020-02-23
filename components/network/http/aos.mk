# component name
NAME := http
# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := http client component
# source files and the folder of internal include files
$(NAME)_INCLUDES += include
$(NAME)_SOURCES := src/http_client.c \
                   wrapper/http_aos_wrapper.c \
                   src/http_method_api.c \
                   src/http_formdata.c

DEPRECATED_PATH := ./deprecated
$(NAME)_INCLUDES += $(DEPRECATED_PATH)/include
$(NAME)_SOURCES += $(DEPRECATED_PATH)/src/http_client.c \
                   $(DEPRECATED_PATH)/src/http_parser.c \
                   $(DEPRECATED_PATH)/src/http_string.c \
                   $(DEPRECATED_PATH)/wrapper/http_aos_wrapper.c

# the folder of API files
GLOBAL_INCLUDES += ../../../include/network/http
# mandatory dependencies
# $(NAME)_COMPONENTS += http_wrapper
# optional dependencies
$(NAME)_COMPONENTS-$(CONFIG_HTTP_SECURE_TLS) += mbedtls
$(NAME)_COMPONENTS-$(CONFIG_HTTP_SECURE_ITLS) += itls
# NOTE
# 1. do NOT import optional dependencies as below
# ifeq (y,$(CONFIG_HTTP_SECURE_TLS))
# $(NAME)_COMPONENTS += mbedtls
# endif
# 2. do NOT define macro in this file. put it into Config.in
# GLOBAL_DEFINES += CONFIG_HTTP_DEBUG

