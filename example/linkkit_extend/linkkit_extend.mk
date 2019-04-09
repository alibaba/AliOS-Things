NAME := linkkit_extend
$(NAME)_SOURCES := app_entry.c

$(NAME)_COMPONENTS += framework/protocol/linkkit/sdk \
                      framework/protocol/linkkit/hal \
                      framework/netmgr \
                      framework/common \
                      utility/cjson \
                      framework/uOTA

GLOBAL_INCLUDES += ../../../../framework/protocol/linkkit/include \
                    ../../../../framework/protocol/linkkit/include/imports \
                    ../../../../framework/protocol/linkkit/include/exports \

GLOBAL_CFLAGS += -DCONFIG_DM_DEVTYPE_SINGLE  \
                 -DCONFIG_YWSS \
                 -DMQTT_DIRECT \
                 -DWIFI_PROVISION_ENABLED \
                 -DAWSS_SUPPORT_DEV_AP \
                 -DOTA_ENABLED \
                 -DBUILD_AOS \
                 -DAWSS_SUPPORT_STATIS \
                 -DMBEDTLS_SSL_MAX_CONTENT_LEN=7168 \

ifeq ($(deprecated),)
$(NAME)_SOURCES += linkkit_extend.c
endif

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

ifeq ($(print_heap),1)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif

ifneq ($(HOST_MCU_FAMILY),esp8266)
$(NAME)_COMPONENTS  += cli
GLOBAL_DEFINES += CONFIG_AOS_CLI
else
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

GLOBAL_INCLUDES += .
