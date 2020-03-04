NAME := http2_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := http2 stream examples

$(NAME)_SOURCES := app_entry.c http2_example_stream.c maintask.c

$(NAME)_COMPONENTS += libiot_http2stream netmgr cjson cli

GLOBAL_DEFINES += ON_DAILY

$(NAME)_INCLUDES += ./

GLOBAL_DEFINES += CONFIG_DM_DEVTYPE_SINGLE  HTTP2_RECV_BUFFER_LENGHT=4096
