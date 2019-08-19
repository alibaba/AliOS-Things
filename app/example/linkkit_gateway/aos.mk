NAME := linkkit_gateway

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := linkkit gateway examples

$(NAME)_SOURCES := app_entry.c 

$(NAME)_COMPONENTS := netmgr cjson
$(NAME)_COMPONENTS += linkkit_sdk_c

GLOBAL_CFLAGS += -DMQTT_DIRECT

ifeq ($(AOS_COMP_UND),y)
$(NAME)_COMPONENTS += und
endif

# LINKKIT_GATEWAY_CONFIG_DEPRECATED ?= n

# LINKKIT_GATEWAY_CONFIG_PRINT_HEAP ?= n
ifeq ($(AOS_COMP_OTA),y)
$(NAME)_COMPONENTS += ota
GLOBAL_DEFINES += ENABLE_AOS_OTA
endif

$(NAME)_SOURCES += linkkit_example_gateway.c

ifeq ($(LINKKIT_GATEWAY_CONFIG_PRINT_HEAP),y)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif


ifneq ($(HOST_MCU_FAMILY),mcu_esp8266)
$(NAME)_COMPONENTS  += cli
else
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

$(NAME)_INCLUDES += ./