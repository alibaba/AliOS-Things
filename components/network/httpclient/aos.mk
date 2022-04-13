NAME := httpc

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := http client component

$(NAME)_SOURCES := ./httpclient.c \
                   ./httpupload.c \
                   ./httpclient_method.c

GLOBAL_INCLUDES += ./

GLOBAL_DEFINES += HTTPCLIENT_ENABLE HTTPCLIENT_SSL_ENABLE
