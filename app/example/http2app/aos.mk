NAME := http2app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := http2 stream examples

$(NAME)_SOURCES := app_entry.c   http2_example_stream.c

$(NAME)_COMPONENTS += linkkit_sdk_c netmgr cjson cli


GLOBAL_DEFINES += ON_DAILY


ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif


GLOBAL_INCLUDES += ./

GLOBAL_DEFINES += CONFIG_DM_DEVTYPE_SINGLE  HTTP2_RECV_BUFFER_LENGHT=4096
