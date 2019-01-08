NAME := linkkit_gateway

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := linkkit gateway examples

$(NAME)_SOURCES := app_entry.c 

$(NAME)_COMPONENTS := netmgr ota cjson
$(NAME)_COMPONENTS += feature.linkkit-gateway

GLOBAL_CFLAGS += -DMQTT_DIRECT

deprecated ?= $(LINKKIT_GATEWAY_CONFIG_DEPRECATED)
print_heap ?= $(LINKKIT_GATEWAY_CONFIG_PRINT_HEAP)

ifneq ($(deprecated),)
$(NAME)_SOURCES += deprecated/gateway.c
GLOBAL_DEFINES += DEPRECATED_LINKKIT
else
$(NAME)_SOURCES += linkkit_example_gateway.c
endif

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif

ifeq ($(print_heap),1)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif


ifneq ($(HOST_MCU_FAMILY),mcu_esp8266)
$(NAME)_COMPONENTS  += cli
else
GLOBAL_DEFINES += ESP8266_CHIPSET
endif

#for test command
ifneq ($(deprecated),)
GLOBAL_CFLAGS += -DLINKKIT_GATEWAY_TEST_CMD
$(NAME)_SOURCES += simulate_subdev/testcmd.c simulate_subdev/testcmd_lock.c
endif
#end

GLOBAL_INCLUDES += ./

